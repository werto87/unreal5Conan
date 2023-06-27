// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProject3GameModeBase.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"

#include "UnrealMacroNuke/UndefineMacros_UE_5.1.0.h"
//
#include "Logging/LogMacros.h"
#include "logic/LogicStateMachine.h"
#include "util/myWebsocket.hxx"
#include <boost/asio.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/certify/https_verification.hpp>
#include <boost/json/src.hpp> //ONLY ONCE IN PROJECT
#include <chrono>
#include <confu_json/confu_json.hxx>
#include <exception>
#include <login_matchmaking_game_shared_type/userMatchmakingSerialization.hxx>
//
#include "UnrealMacroNuke/RedefineMacros_UE_5.1.0.h"

#pragma clang diagnostic pop

using CoroTimer = boost::asio::use_awaitable_t<>::as_default_on_t<boost::asio::basic_waitable_timer<boost::asio::chrono::system_clock> >;
using Websocket = boost::beast::websocket::stream<boost::asio::use_awaitable_t<>::as_default_on_t<boost::beast::tcp_stream> >;

boost::asio::awaitable<void>
connectToModernDurak (boost::asio::io_context &ioContext, std::vector<std::string> sendMessageBeforeStartRead = {}, std::optional<std::string> connectionName = {})
{
  try
    {
      using namespace boost::asio;
      using namespace boost::beast;
      try
        {
          using SSLWebsocket = websocket::stream<ssl_stream<tcp_stream> >;
          ssl::context ctx { ssl::context::tlsv12_client };
          auto address = std::string { "www.modern-durak.com" };
          auto connection = std::make_shared<SSLWebsocket> (SSLWebsocket { ioContext, ctx });
          get_lowest_layer (*connection).expires_never ();
          connection->set_option (websocket::stream_base::timeout::suggested (role_type::client));
          connection->set_option (websocket::stream_base::decorator ([] (websocket::request_type &req) { req.set (http::field::user_agent, std::string (BOOST_BEAST_VERSION_STRING) + " websocket-client-async-ssl"); }));
          using ip::tcp;
          tcp::resolver::query myQuery (address, "https");
          tcp::resolver resolver { ioContext };
          tcp::resolver::iterator endpoint_iterator = resolver.resolve (myQuery);
          co_await get_lowest_layer (*connection).async_connect (*endpoint_iterator, use_awaitable);
          co_await connection->next_layer ().async_handshake (ssl::stream_base::client, use_awaitable);
          co_await connection->async_handshake (endpoint_iterator->endpoint ().address ().to_string () + ":" + std::to_string (endpoint_iterator->endpoint ().port ()), "/wss", use_awaitable);
          static size_t id = 0;
          auto myWebsocket = std::make_shared<MyWebsocket<SSLWebsocket> > (MyWebsocket<SSLWebsocket> { std::move (connection), connectionName ? connectionName.value () : std::string { "connectWebsocket" }, fg (fmt::color::chocolate), std::to_string (id++) });
          for (auto message : sendMessageBeforeStartRead)
            {
              co_await myWebsocket->async_write_one_message (message);
            }
          using namespace experimental::awaitable_operators;
          auto logicStateMachine = LogicStateMachine {};
          co_await (myWebsocket->readLoop ([&logicStateMachine] (std::string const &msg) {
            UE_LOG (LogTemp, Warning, TEXT ("message from matchmaking: %s"), *FString { msg.c_str () });
            if (auto error = logicStateMachine.processEvent (msg))
              {
                UE_LOG (LogTemp, Error, TEXT ("errorHandleMessageFromGame: %s"), *FString { error->c_str () });
              }
          }) && myWebsocket->writeLoop ());
        }
      catch (std::exception const &e)
        {
          UE_LOG (LogTemp, Error, TEXT ("connectWebsocketSSL () connect  Exception : %s"), *FString { e.what () });
        }
    }
  catch (std::exception const &e)
    {
      UE_LOG (LogTemp, Error, TEXT ("connectWebsocketSSL () connect  Exception : %s"), *FString { e.what () });
    }
}

boost::asio::awaitable<void>
connectToLocalWebsocket (boost::asio::io_context &ioContext, std::vector<std::string> sendMessageBeforeStartRead = {}, std::optional<std::string> connectionName = {})
{
  try
    {
      using namespace boost::asio;
      using namespace boost::beast;
      try
        {
          using SSLWebsocket = websocket::stream<ssl_stream<tcp_stream> >;
          ssl::context ctx { ssl::context::tlsv12_client };
          auto address = std::string {};
          ctx.set_verify_mode (ssl::verify_none); // DO NOT USE THIS IN PRODUCTION THIS WILL IGNORE CHECKING FOR TRUSTFUL CERTIFICATE
          address = std::string { "localhost:55555" };
          auto connection = std::make_shared<SSLWebsocket> (SSLWebsocket { ioContext, ctx });
          get_lowest_layer (*connection).expires_never ();
          connection->set_option (websocket::stream_base::timeout::suggested (role_type::client));
          connection->set_option (websocket::stream_base::decorator ([] (websocket::request_type &req) { req.set (http::field::user_agent, std::string (BOOST_BEAST_VERSION_STRING) + " websocket-client-async-ssl"); }));
          using ip::tcp;
          auto endpoint = tcp::endpoint { tcp::v4 (), 55555 };
          co_await get_lowest_layer (*connection).async_connect (endpoint, use_awaitable);
          co_await connection->next_layer ().async_handshake (ssl::stream_base::client, use_awaitable);
          co_await connection->async_handshake (endpoint.address ().to_string () + ":" + std::to_string (endpoint.port ()), "/", use_awaitable);
          static size_t id = 0;
          auto myWebsocket = std::make_shared<MyWebsocket<SSLWebsocket> > (MyWebsocket<SSLWebsocket> { std::move (connection), connectionName ? connectionName.value () : std::string { "connectWebsocket" }, fg (fmt::color::chocolate), std::to_string (id++) });
          for (auto message : sendMessageBeforeStartRead)
            {
              co_await myWebsocket->async_write_one_message (message);
            }
          using namespace experimental::awaitable_operators;
          auto logicStateMachine = LogicStateMachine {};
          co_await (myWebsocket->readLoop ([&logicStateMachine] (std::string const &msg) {
            UE_LOG (LogTemp, Warning, TEXT ("message from matchmaking: %s"), *FString { msg.c_str () });
            if (auto error = logicStateMachine.processEvent (msg))
              {
                UE_LOG (LogTemp, Error, TEXT ("errorHandleMessageFromGame: %s"), *FString { error->c_str () });
              }
          }) && myWebsocket->writeLoop ());
        }
      catch (std::exception const &e)
        {
          UE_LOG (LogTemp, Error, TEXT ("connectWebsocketSSL () connect  Exception : %s"), *FString { e.what () });
        }
    }
  catch (std::exception const &e)
    {
      UE_LOG (LogTemp, Error, TEXT ("connectWebsocketSSL () connect  Exception : %s"), *FString { e.what () });
    }
}

void
printExceptionHelper (std::exception_ptr eptr)
{
  try
    {
      if (eptr)
        {
          std::rethrow_exception (eptr);
        }
    }
  catch (std::exception const &e)
    {
      UE_LOG (LogTemp, Error, TEXT ("co_spawn exception: %s"), *FString { e.what () });
    }
}

template <class... Fs> struct overloaded : Fs...
{
  using Fs::operator()...;
};

template <class... Fs> overloaded (Fs...) -> overloaded<Fs...>;

auto const printException1 = [] (std::exception_ptr eptr) { printExceptionHelper (eptr); };

auto const printException2 = [] (std::exception_ptr eptr, auto) { printExceptionHelper (eptr); };

auto const printException = overloaded { printException1, printException2 };

AMyProject3GameModeBase::AMyProject3GameModeBase () { PrimaryActorTick.bCanEverTick = true; }

// Called when the game starts or when spawned
void
AMyProject3GameModeBase::BeginPlay ()
{
  Super::BeginPlay ();
  using namespace boost::asio;
  auto sendMessageBeforeStartRead = std::vector<std::string> { { "LoginAsGuest|{}" } };
  auto isLocalWebsocket = false;
  if (isLocalWebsocket)
    {
      co_spawn (ioContext, connectToLocalWebsocket (ioContext, sendMessageBeforeStartRead, "test"), printException);
    }
  else
    {
      co_spawn (ioContext, connectToModernDurak (ioContext, sendMessageBeforeStartRead, "test"), printException);
    }
}

void
AMyProject3GameModeBase::Tick (float DeltaSeconds)
{

  Super::Tick (DeltaSeconds);
  ioContext.poll_one ();
}