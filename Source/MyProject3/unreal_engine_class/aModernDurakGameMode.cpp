// Copyright Epic Games, Inc. All Rights Reserved.

#include "aModernDurakGameMode.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"

#include "MyProject3/unreal_engine_class/aModernDurakGameState.h"

/*
 *
 */
#include "MyProject3/UnrealMacroNuke/UndefineMacros_UE_5.1.0.h"
//

#include "Logging/LogMacros.h"
#include "MyProject3/logic/Game.h"

#include "MyProject3/util/myWebsocket.h"
#include "MyProject3/util/util.h"

#include <boost/asio.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/certify/https_verification.hpp>
#include <boost/json/src.hpp> //ONLY ONCE IN PROJECT
#include <chrono>
#include <exception>
#include <login_matchmaking_game_shared_type/userMatchmakingSerialization.hxx>
//
#include "MyProject3/UnrealMacroNuke/RedefineMacros_UE_5.1.0.h"

#pragma clang diagnostic pop

using CoroTimer = boost::asio::use_awaitable_t<>::as_default_on_t<boost::asio::basic_waitable_timer<boost::asio::chrono::system_clock> >;
using Websocket = boost::beast::websocket::stream<boost::asio::use_awaitable_t<>::as_default_on_t<boost::beast::tcp_stream> >;
using SSLWebsocket = boost::beast::websocket::stream<boost::beast::ssl_stream<boost::beast::tcp_stream> >;

AModernDurakGameMode::AModernDurakGameMode () { PrimaryActorTick.bCanEverTick = true; }

boost::asio::awaitable<void>
processMessage (std::function<void (FString const &message)> onMessage, std::shared_ptr<SSLWebsocket> &connection, std::optional<std::string> const &connectionName, std::vector<std::string> const &sendMessageBeforeStartRead)
{
  using namespace boost::asio;
  using namespace boost::beast;
  static size_t id = 0;
  auto myWebsocket = std::make_shared<MyWebsocket<SSLWebsocket> > (MyWebsocket<SSLWebsocket> { std::move (connection), connectionName ? connectionName.value () : std::string { "connectWebsocket" }, fg (fmt::color::chocolate), std::to_string (id++) });
  for (auto &&message : sendMessageBeforeStartRead)
    {
      co_await myWebsocket->async_write_one_message (std::move (message));
    }
  using namespace experimental::awaitable_operators;
  auto logicStateMachine = GameLogicStateMachine { LogicStateMachineDependencies { onMessage } };
  co_await (myWebsocket->readLoop ([&logicStateMachine] (std::string const &msg) {
    UE_LOG (LogTemp, Warning, TEXT ("message from matchmaking: %s"), *FString { msg.c_str () });
    if (auto error = logicStateMachine.processEvent (msg))
      {
        UE_LOG (LogTemp, Error, TEXT ("errorHandleMessageFromGame: %s"), *FString { error->c_str () });
      }
  }) && myWebsocket->writeLoop ());
}

boost::asio::awaitable<void>
AModernDurakGameMode::connectToModernDurak (std::vector<std::string> sendMessageBeforeStartRead, std::optional<std::string> connectionName)
{
  try
    {
      using namespace boost::asio;
      using namespace boost::beast;
      try
        {
          ssl::context ctx { ssl::context::tlsv12_client };
          auto const address = std::string { "www.modern-durak.com" };
          auto connection = std::make_shared<SSLWebsocket> (SSLWebsocket { ioContext, ctx });
          get_lowest_layer (*connection).expires_never ();
          connection->set_option (websocket::stream_base::timeout::suggested (role_type::client));
          connection->set_option (websocket::stream_base::decorator ([] (websocket::request_type &req) { req.set (http::field::user_agent, std::string (BOOST_BEAST_VERSION_STRING) + " websocket-client-async-ssl"); }));
          using ip::tcp;
          const tcp::resolver::query myQuery (address, "https");
          tcp::resolver resolver { ioContext };
          const tcp::resolver::iterator endpoint_iterator = resolver.resolve (myQuery);
          co_await get_lowest_layer (*connection).async_connect (*endpoint_iterator, use_awaitable);
          co_await connection->next_layer ().async_handshake (ssl::stream_base::client, use_awaitable);
          co_await connection->async_handshake (endpoint_iterator->endpoint ().address ().to_string () + ":" + std::to_string (endpoint_iterator->endpoint ().port ()), "/wss", use_awaitable);
          co_await processMessage ([this] (FString const &message) { GetGameState<AModernDurakGameState> ()->newMessageFromRemote.Broadcast (message); }, connection, connectionName, sendMessageBeforeStartRead);
        }
      catch (std::exception const &e)
        {
          UE_LOG (LogTemp, Error, TEXT ("Exception : %s"), *FString { e.what () });
        }
    }
  catch (std::exception const &e)
    {
      UE_LOG (LogTemp, Error, TEXT ("Exception : %s"), *FString { e.what () });
    }
}

boost::asio::awaitable<void>
AModernDurakGameMode::connectToLocalWebsocket (std::vector<std::string> sendMessageBeforeStartRead, std::optional<std::string> connectionName)
{
  try
    {
      using namespace boost::asio;
      using namespace boost::beast;
      try
        {
          ssl::context ctx { ssl::context::tlsv12_client };
          ctx.set_verify_mode (ssl::verify_none); // DO NOT USE THIS IN PRODUCTION THIS WILL IGNORE CHECKING FOR TRUSTFUL CERTIFICATE
          auto connection = std::make_shared<SSLWebsocket> (SSLWebsocket { ioContext, ctx });
          get_lowest_layer (*connection).expires_never ();
          connection->set_option (websocket::stream_base::timeout::suggested (role_type::client));
          connection->set_option (websocket::stream_base::decorator ([] (websocket::request_type &req) { req.set (http::field::user_agent, std::string (BOOST_BEAST_VERSION_STRING) + " websocket-client-async-ssl"); }));
          using ip::tcp;
          auto endpoint = tcp::endpoint { tcp::v4 (), 55555 };
          co_await get_lowest_layer (*connection).async_connect (endpoint, use_awaitable);
          co_await connection->next_layer ().async_handshake (ssl::stream_base::client, use_awaitable);
          co_await connection->async_handshake (endpoint.address ().to_string () + ":" + std::to_string (endpoint.port ()), "/", use_awaitable);
          co_await processMessage ([this] (FString const &message) { GetGameState<AModernDurakGameState> ()->newMessageFromRemote.Broadcast (message); }, connection, connectionName, sendMessageBeforeStartRead);
        }
      catch (std::exception const &e)
        {
          UE_LOG (LogTemp, Error, TEXT ("Exception : %s"), *FString { e.what () });
        }
    }
  catch (std::exception const &e)
    {
      UE_LOG (LogTemp, Error, TEXT ("Exception : %s"), *FString { e.what () });
    }
}

// Called when the game starts or when spawned
void
AModernDurakGameMode::BeginPlay ()
{
  Super::BeginPlay ();
  using namespace boost::asio;
  auto sendMessageBeforeStartRead = std::vector<std::string> { { "LoginAsGuest|{}" } };
#ifdef RUN_AGAINST_REMOTE
  co_spawn (ioContext, connectToModernDurak (sendMessageBeforeStartRead, "test"), printException);
#else
  co_spawn (ioContext, connectToLocalWebsocket (sendMessageBeforeStartRead, "test"), printException);
#endif
}

void
AModernDurakGameMode::Tick (float DeltaSeconds)
{
  Super::Tick (DeltaSeconds);
  ioContext.poll_one ();
}