// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProject3GameModeBase.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include "UnrealMacroNuke/UndefineMacros_UE_5.1.0.h"
#include "util/myWebsocket.hxx"
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio.hpp>
#include <exception>
#include <boost/beast.hpp>
#include "chrono"
#include "UnrealMacroNuke/RedefineMacros_UE_5.1.0.h"
#pragma clang diagnostic pop

typedef boost::asio::use_awaitable_t<>::as_default_on_t<boost::asio::basic_waitable_timer<boost::asio::chrono::system_clock>> CoroTimer;
typedef boost::beast::websocket::stream<boost::asio::use_awaitable_t<>::as_default_on_t<boost::beast::tcp_stream>> Websocket;
AMyProject3GameModeBase::AMyProject3GameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

boost::asio::awaitable<void>
sendStartGameToServer (boost::asio::io_context &ioContext)
{
	//TODO send a message to matchmaking to start a game
	//TODO note needs ssl. Some code should be found in tests for matchmaking.
	UE_LOG(LogTemp, Warning, TEXT("sendStartGameToServer1"));
	auto ws = std::make_shared<Websocket> (ioContext);
	auto matchmakingGameEndpoint = boost::asio::ip::tcp::endpoint{ boost::asio::ip::tcp::v4 (), 4242 };
	co_await ws->next_layer ().async_connect (matchmakingGameEndpoint);
	UE_LOG(LogTemp, Warning, TEXT("sendStartGameToServer2"));
	ws->next_layer ().expires_never ();
	ws->set_option (boost::beast::websocket::stream_base::timeout::suggested (boost::beast::role_type::client));
	ws->set_option (boost::beast::websocket::stream_base::decorator ([] (boost::beast::websocket::request_type &req) { req.set (boost::beast::http::field::user_agent, std::string (BOOST_BEAST_VERSION_STRING) + " websocket-client-async"); }));
	co_await ws->async_handshake (matchmakingGameEndpoint.address ().to_string () + std::to_string (matchmakingGameEndpoint.port ()), "/");
	UE_LOG(LogTemp, Warning, TEXT("sendStartGameToServer3"));
	static size_t id = 0;
	auto myWebsocket = MyWebsocket<Websocket>{ std::move (ws), "sendStartGameToServer", fmt::fg (fmt::color::cornflower_blue), std::to_string (id++) };
	co_await myWebsocket.async_write_one_message (R"foo(StartGame|{"players":[],"gameOption":{"gameOption":{"maxCardValue":9,"typeCount":4,"numberOfCardsPlayerShouldHave":6,"roundToStart":1,"trump":null,"customCardDeck":null,"cardsInHands":null},"timerOption":{"timerType":"noTimer","timeAtStartInSeconds":0,"timeForEachRoundInSeconds":0},"computerControlledPlayerCount":0,"opponentCards":"showNumberOfOpponentCards"},"ratedGame":false})foo");
	UE_LOG(LogTemp, Warning, TEXT("sendStartGameToServer4"));
	auto msg = co_await myWebsocket.async_read_one_message ();
	UE_LOG(LogTemp, Warning, TEXT("sendStartGameToServer5"));
	UE_LOG(LogTemp, Warning, TEXT("%s"),*FString{msg.c_str()});
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
		UE_LOG(LogTemp, Error, TEXT("co_spawn exception: %s"), *FString{e.what ()});
		// std::cout << "co_spawn exception: '" << e.what () << "'" << std::endl;
	}
}

template <class... Fs> struct overloaded : Fs...
{
	using Fs::operator()...;
};

template <class... Fs> overloaded (Fs...) -> overloaded<Fs...>;

auto const printException1 = [] (std::exception_ptr eptr) { printExceptionHelper (eptr); };

auto const printException2 = [] (std::exception_ptr eptr, auto) { printExceptionHelper (eptr); };

auto const printException = overloaded{ printException1, printException2 };
// Called when the game starts or when spawned
void AMyProject3GameModeBase::BeginPlay()
{
	Super::BeginPlay();
	boost::asio::co_spawn(ioContext,sendStartGameToServer(ioContext),printException);
}


void AMyProject3GameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	ioContext.poll_one();
}
