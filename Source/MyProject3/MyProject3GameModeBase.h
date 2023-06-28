// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include <boost/asio/awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <optional>
#include <string>
/**
 *
 */
#include "MyProject3GameModeBase.generated.h"
UCLASS ()
class MYPROJECT3_API AMyProject3GameModeBase : public AGameModeBase
{
  GENERATED_BODY ()
public:
  AMyProject3GameModeBase ();
  virtual void BeginPlay () override;
  virtual void Tick (float DeltaSeconds) override;

private:
  boost::asio::awaitable<void> connectToModernDurak (std::vector<std::string> sendMessageBeforeStartRead = {}, std::optional<std::string> connectionName = {});
  boost::asio::awaitable<void> connectToLocalWebsocket (std::vector<std::string> sendMessageBeforeStartRead = {}, std::optional<std::string> connectionName = {});

  UPROPERTY (VisibleAnywhere, meta = (AllowPrivateAccess = "true"), BlueprintReadOnly)
  FString helloWorld { "Hello World!" };
  boost::asio::io_context ioContext {};
};