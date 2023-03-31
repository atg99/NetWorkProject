// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OnlineSubsystem.h"
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "ServerGameInstance.generated.h"

/**
 * 
 */
USTRUCT()
struct FSessionInfo
{
	GENERATED_BODY()

	FString roomName;
	int32 currentPlayers;
	int32 maxPlayers;
	int32 ping;
	int32 idx;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSearchResult, FSessionInfo, sessionInfo); //델리게이트 선언 UCLASS 위에
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSearchFinshed); //델리게이트 선언 UCLASS 위에

UCLASS()

class NETWORKPROJECT_API UServerGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	//생성자
	UServerGameInstance();
	
	//비긴 플레이
	virtual void Init() override;

	//UPROPERTY()
	IOnlineSessionPtr sessionInterface;

	//함수
	UFUNCTION()
	void CreateMySession(FString roomName, int32 playerCount);

	UFUNCTION()
	void OnCreateSessionComplete(FName sessionName, bool bIsSuccess);

	UFUNCTION()
	void FindMySession();

	UFUNCTION()
	void JoinMySession(int32 sessionIDX);

	UFUNCTION()
	void OnFindSessionComplete(bool bWasSuccessful);

	UPROPERTY()
	FName sessionID;

	FOnSearchResult searchResultDele;

	FOnSearchFinshed searchFinshed;
	
	class TSharedPtr<FOnlineSessionSearch> sessionSearch;
	
	void OnJoinSessionComplete(FName sessionName, enum EOnJoinSessionCompleteResult::Type joinResult);

	// UPROPERTY(EditAnywhere)
	// TSubclassOf<class USessionSlotWidget> sessionSlot;
	
};

