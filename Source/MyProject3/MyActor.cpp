// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActor.h"
#include "my_package/hello.hxx"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
// #pragma clang diagnostic ignored "-Wundef"
// #pragma clang diagnostic ignored "-Wshadow"
// #pragma clang diagnostic ignored "-Wunused-result"
// #include <coroutine>
#include "UnrealMacroNuke/UndefineMacros_UE_5.1.0.h"
#include <boost/asio/io_context.hpp>
// #include <chrono>
#include <boost/asio/post.hpp>
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/optional.hpp>
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/optional.hpp>
// #include <boost/chrono.hpp>
#include "boost/asio/co_spawn.hpp"
#include "chrono"
#include "UnrealMacroNuke/RedefineMacros_UE_5.1.0.h"
#pragma clang diagnostic pop


// Sets default values
// typedef boost::asio::use_awaitable_t<>::as_default_on_t<boost::asio::basic_waitable_timer<boost::asio::chrono::system_clock> > CoroTimer;
AMyActor::AMyActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

boost::asio::awaitable<void> writeEverySecond(boost::asio::io_context& ioContext)
{
	for(;;)
	{
		auto timer=boost::asio::steady_timer{ioContext};
		timer.expires_after(std::chrono::seconds{1});
		co_await timer.async_wait(boost::asio::use_awaitable);
		UE_LOG(LogTemp, Warning, TEXT("The Actor's name is %s"), *FString{helloWorld().c_str()});
	}
}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();
	auto ioContext=boost::asio::io_context{};
	//TODO find a way to put ioContext in unreal engine
	boost::asio::co_spawn(ioContext,writeEverySecond(ioContext),boost::asio::detached);
	ioContext.run_for(std::chrono::seconds{10});
}

// Called every frame
void AMyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
