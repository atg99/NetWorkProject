// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKPROJECT_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_roomName;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_PlayerCount;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_CreateSession;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_Start;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_GoCreate;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_CreateBack;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_GoJoin;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_JoinBack;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_refresh;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UEditableText* ed_ID;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UEditableText* ed_RoomName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UWidgetSwitcher* ws_ws;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class USlider* sl_PlayerCount;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UScrollBox* sbox_roomListSrollBox;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class USessionSlotWidget> sessionSlot;
	
protected:
	virtual void NativeConstruct() override;

private:
	class UServerGameInstance* gameInstance;
	
	UFUNCTION()
	void ClickStart();

	UFUNCTION()
	void CreateServer();

	UFUNCTION()
	void OnMoveSlider(float Value);

	UFUNCTION()
	void OnColorChange();
	
	UFUNCTION()
	void OnUnHoveredStart();

	UFUNCTION()
	void OnClickGoCreate();

	UFUNCTION()
	void OnClickGoJoin();

	UFUNCTION()
	void RefreshList();

	UFUNCTION()
	void GoBack();

	UFUNCTION()
	void AddNewSlot(FSessionInfo sessionInfo);

	UFUNCTION()
	void RefreshEnabled();
	
	UPROPERTY()
	FLinearColor startColor;
};


