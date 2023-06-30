// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include <boost/asio/awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <cstdint>
#include <optional>
#include <string>
/**
 *
 */
#include "aModernDurakGameMode.generated.h"

UCLASS ()
class MYPROJECT3_API AModernDurakGameMode : public AGameModeBase
{
  GENERATED_BODY ()
public:
  AModernDurakGameMode ();
  virtual void BeginPlay () override;
  virtual void Tick (float DeltaSeconds) override;

private:
  boost::asio::awaitable<void> connectToModernDurak (std::vector<std::string> sendMessageBeforeStartRead = {}, std::optional<std::string> connectionName = {});
  boost::asio::awaitable<void> connectToLocalWebsocket (std::vector<std::string> sendMessageBeforeStartRead = {}, std::optional<std::string> connectionName = {});
  boost::asio::io_context ioContext {};
};