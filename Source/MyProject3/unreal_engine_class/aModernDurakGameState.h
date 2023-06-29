// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameStateBase.h"
#include <cstdint>
/*
 *
 */
#include "aModernDurakGameState.generated.h"

UCLASS ()
class MYPROJECT3_API AModernDurakGameState : public AGameStateBase
{
  GENERATED_BODY ()

public:
  UPROPERTY (EditAnywhere, BlueprintReadWrite)
  FString lastMessageFromRemote {};
};