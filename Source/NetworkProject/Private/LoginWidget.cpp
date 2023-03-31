// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"

#include "ServerGameInstance.h"
#include "SessionSlotWidget.h"
#include "Components/Slider.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/WidgetSwitcher.h"

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ed_ID->SetText(FText::FromString(""));
	ed_RoomName->SetText(FText::FromString(""));
	sl_PlayerCount->SetValue(2.0f);
	text_PlayerCount->SetText(FText::FromString("2"));
	
	//버튼 클릭 이벤트에 함수 바인딩
	btn_Start->OnClicked.AddDynamic(this, &ULoginWidget::ClickStart);
	btn_CreateSession->OnClicked.AddDynamic(this, &ULoginWidget::CreateServer);
	sl_PlayerCount->OnValueChanged.AddDynamic(this, &ULoginWidget::OnMoveSlider);

	btn_Start->OnHovered.AddDynamic(this, &ULoginWidget::OnColorChange);
	btn_Start->OnUnhovered.AddDynamic(this, &ULoginWidget::OnUnHoveredStart);

	btn_GoCreate->OnClicked.AddDynamic(this, &ULoginWidget::OnClickGoCreate);
	btn_GoJoin->OnClicked.AddDynamic(this, &ULoginWidget::OnClickGoJoin);
	
	btn_CreateBack->OnClicked.AddDynamic(this, &ULoginWidget::GoBack);
	btn_JoinBack->OnClicked.AddDynamic(this, &ULoginWidget::GoBack);
	btn_refresh->OnClicked.AddDynamic(this, &ULoginWidget::RefreshList);
	
	//게임 인스턴스는 모든 클레스에서 바로 가져올 수 있다.
	gameInstance = Cast<UServerGameInstance>(GetGameInstance());

	if(gameInstance)
	{
		gameInstance->searchResultDele.AddDynamic(this, &ULoginWidget::AddNewSlot);
		gameInstance->searchFinshed.AddDynamic(this, &ULoginWidget::RefreshEnabled);
	}
}

void ULoginWidget::ClickStart()
{
	//만일, ID가 빈칸이 아니라면
	if(!ed_ID->GetText().IsEmpty())
	{
		ws_ws->SetActiveWidgetIndex(1);
		//FName은 Text를 Tostring으로 변경해야 받을 수 있다
		gameInstance->sessionID = FName(*ed_ID->GetText().ToString());
	}
}

void ULoginWidget::CreateServer()
{
	int32 playerCunt = FMath::RoundHalfFromZero(sl_PlayerCount->GetValue());
	gameInstance->CreateMySession(ed_RoomName->GetText().ToString(), playerCunt);
}

//슬라이더 값이 변경 될 때 호출 되는 함수
void ULoginWidget::OnMoveSlider(float Value)
{
	FString num2str = FString::Printf(TEXT("%d"), sl_PlayerCount->GetValue());
	text_PlayerCount->SetText(FText::AsNumber(FMath::Floor(Value)));
}

void ULoginWidget::OnColorChange()
{
	UE_LOG(LogTemp, Warning, TEXT("onhover"));
	startColor = btn_Start->BackgroundColor;
	btn_Start->SetColorAndOpacity(FLinearColor::Green);
}

void ULoginWidget::OnUnHoveredStart()
{
	btn_Start->SetColorAndOpacity(startColor);
}

void ULoginWidget::OnClickGoCreate()
{
	ws_ws->SetActiveWidgetIndex(2);
}

void ULoginWidget::OnClickGoJoin()
{
	ws_ws->SetActiveWidgetIndex(3);
	RefreshList();
}

//세션 리스트 새로고침 함수
void ULoginWidget::RefreshList()
{
	sbox_roomListSrollBox->ClearChildren();
	gameInstance->FindMySession();
	btn_refresh->SetIsEnabled(false);
}

void ULoginWidget::GoBack()
{
	ws_ws->SetActiveWidgetIndex(1);
}

//게임 이벤트로 부터 검색 완료 이벤트를 받았을 때 실행될 함수
void ULoginWidget::AddNewSlot(FSessionInfo sessionInfo)
{
	USessionSlotWidget* slotWidget = CreateWidget<USessionSlotWidget>(this, sessionSlot);
	if(!slotWidget) { return; }
	slotWidget->text_roomName->SetText(FText::FromString(sessionInfo.roomName));
	slotWidget->text_playerInfo->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), sessionInfo.currentPlayers, sessionInfo.maxPlayers)));
	slotWidget->text_ping->SetText(FText::FromString(FString::Printf(TEXT("%d ms"), sessionInfo.ping)));
	slotWidget->index = sessionInfo.idx;
	sbox_roomListSrollBox->AddChild(slotWidget);
}

void ULoginWidget::RefreshEnabled()
{
	btn_refresh->SetIsEnabled(true);
}
