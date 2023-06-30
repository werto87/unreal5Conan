// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameStateBase.h"
#include <cstdint>
/*
 *
 */
#include "aModernDurakGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FNewMessageFromRemote, FString, message);

UCLASS ()

class MYPROJECT3_API AModernDurakGameState : public AGameStateBase
{
  GENERATED_BODY ()

public:
  UPROPERTY (BlueprintAssignable, Category = "ModernDurak")
  FNewMessageFromRemote newMessageFromRemote;
};