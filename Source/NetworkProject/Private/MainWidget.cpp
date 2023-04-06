// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"

#include "BattleGameStateBase.h"
#include "NetworkPlayer.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"



void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	player = Cast<ANetworkPlayer>(GetOwningPlayerPawn());

	btn_exit->OnClicked.AddDynamic(this, &UMainWidget::ExitSession);
}

void UMainWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	text_health->SetText(FText::AsNumber(player->GetHealth()));
	text_Ammo->SetText(FText::AsNumber(player->GetAmmo()));

	FString playerList;

	if(!GetWorld()->GetGameState()){ return; }

	ABattleGameStateBase* gs = Cast<ABattleGameStateBase>(GetWorld()->GetGameState());

	for(const auto& ps : gs->GetPlayerListByScore())
	{
		FString playerName = ps->GetPlayerName();
		int32 playerScore = ps->GetScore();
		playerList.Append(FString::Printf(TEXT("%s Score : %d\n"), *playerName, playerScore));
	}
	text_PlayerList->SetText(FText::FromString(playerList));
}

void UMainWidget::ExitSession()
{
	if(player)
	{
		player->EndSession();
	}
}

