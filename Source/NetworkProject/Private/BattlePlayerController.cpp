// Fill out your copyright notice in the Description page of Project Settings.


#include "BattlePlayerController.h"
#include "MainWidget.h"
#include "Blueprint/UserWidget.h"

void ABattlePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(mainWidget && IsLocalController())
	{
		UMainWidget* mainUI = CreateWidget<UMainWidget>(this, mainWidget);
		if(mainUI)
		{
			mainUI->AddToViewport();
		}
	}
}
