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

//ĳ���� ��Ȱ �Լ�
void ABattlePlayerController::Respawn(ANetworkPlayer* player)
{
	//�������� ���� ���Ӹ��� ������ �ִ�
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
