// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"
#include "NetworkPlayer.h"
#include "Components/TextBlock.h"


void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	player = Cast<ANetworkPlayer>(GetOwningPlayerPawn());
}

void UMainWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	text_health->SetText(FText::AsNumber(player->GetHealth()));
	text_Ammo->SetText(FText::AsNumber(player->GetAmmo()));
}
