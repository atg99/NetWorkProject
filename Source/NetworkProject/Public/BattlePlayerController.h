// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BattlePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKPROJECT_API ABattlePlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMainWidget> mainWidget;

public:

	//posess된 플레이어를 받는다
	UFUNCTION()
	void Respawn(class ANetworkPlayer* player);
	
};
