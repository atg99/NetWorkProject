// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"

#include "NetworkPlayer.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"


void UMyUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	player = Cast<ANetworkPlayer>(GetOwningPlayerPawn());
	if(player)
	{
		//ServerSetHeathBar(player->GetHealth());
	}
}

void UMyUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if(!player)
	{
		return;
	}
	//float calcHP = (float)player->GetHealth() / (float)player->maxHP;
	//pb_HP->SetPercent(calcHP);
	
}

void UMyUserWidget::SetPlayer(ANetworkPlayer* getPlayer)
{
	this->player = getPlayer;
}

void UMyUserWidget::MultiCastSetHeathBar_Implementation(const int32 val)
{
	
	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, TEXT("setpersent"));
	//float calcHP = (float)player->GetHealth()/ (float)player->maxHP;
	//pb_HP->SetPercent(calcHP);
}


void UMyUserWidget::ServerSetHeathBar_Implementation(const int32 val)
{
	//FTimerHandle timer;

	//GetWorld()->GetTimerManager().SetTimer(timer, FTimerDelegate::CreateLambda([this](void) { MultiCastSetHeathBar(10); }), 0, false, 1);
}
