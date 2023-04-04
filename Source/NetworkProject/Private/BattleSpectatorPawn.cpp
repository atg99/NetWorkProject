// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleSpectatorPawn.h"

#include "BattlePlayerController.h"

void ABattleSpectatorPawn::BeginPlay()
{
	Super::BeginPlay();

}

void ABattleSpectatorPawn::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
}
