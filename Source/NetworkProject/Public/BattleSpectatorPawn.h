// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "BattleSpectatorPawn.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKPROJECT_API ABattleSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
};
