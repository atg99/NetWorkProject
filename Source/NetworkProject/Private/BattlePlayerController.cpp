// Fill out your copyright notice in the Description page of Project Settings.


#include "BattlePlayerController.h"

#include "BattleGameMode.h"
#include "BattleSpectatorPawn.h"
#include "MainWidget.h"
#include "NetworkPlayer.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameModeBase.h"

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

//캐릭터 부활 함수
void ABattlePlayerController::Respawn(ANetworkPlayer* player)
{
	//서버에서 실행 게임모드는 서버만 있다
	if(HasAuthority() && player)
	{
		ABattleGameMode* gm = Cast<ABattleGameMode>(GetWorld()->GetAuthGameMode());
		if(gm)
		{
			player->Destroy();
			
			gm->RestartPlayer(this);
		}
	}
}
