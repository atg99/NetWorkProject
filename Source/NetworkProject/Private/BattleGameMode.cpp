// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleGameMode.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"

AActor* ABattleGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	//월드 안에 있는 APlayerStart 엑터들을 가져온다.
	for(TActorIterator<APlayerStart> iter(GetWorld()); iter; ++iter)
	{
		APlayerStart* ps = *iter;
		if(ps->PlayerStartTag != FName("Spawned"))
		{
			//반환될 APlayerStart 엑터의 테그를 "Spawed" 추가한다.
			ps->PlayerStartTag = FName("Spawned");
			return ps;
		}
	}

	//플레이어수가 플레이어스타트 엑터보다 많으면 null이 반환된다 (위험)
	return nullptr;
}
