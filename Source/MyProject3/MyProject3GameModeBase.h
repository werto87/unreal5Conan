// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include <boost/asio/io_context.hpp>

#include "MyProject3GameModeBase.generated.h"
/**
 *
 */
UCLASS ()
class MYPROJECT3_API AMyProject3GameModeBase : public AGameModeBase
{
  GENERATED_BODY ()
public:
  AMyProject3GameModeBase ();
  virtual void BeginPlay () override;
  virtual void Tick (float DeltaSeconds) override;

  void myFunc ();
  boost::asio::io_context ioContext{};
};