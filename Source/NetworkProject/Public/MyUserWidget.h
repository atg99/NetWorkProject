// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKPROJECT_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* text_name;

	UPROPERTY(meta=(BindWidget))
	class UProgressBar* pb_HP;

	UFUNCTION()
	void SetPlayer(ANetworkPlayer* getPlayer);

	UFUNCTION(Server, Unreliable)
		void ServerSetHeathBar(const int32 val);

	UFUNCTION(Server, Unreliable)
		void MultiCastSetHeathBar(const int32 val);

private:
	class ANetworkPlayer* player;
};



