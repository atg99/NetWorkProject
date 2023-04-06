// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleGameMode.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"

AActor* ABattleGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	//���� �ȿ� �ִ� APlayerStart ���͵��� �����´�.
	for(TActorIterator<APlayerStart> iter(GetWorld()); iter; ++iter)
	{
		APlayerStart* ps = *iter;
		if(ps->PlayerStartTag != FName("Spawned"))
		{
			//��ȯ�� APlayerStart ������ �ױ׸� "Spawed" �߰��Ѵ�.
			ps->PlayerStartTag = FName("Spawned");
			return ps;
		}
	}

	//�÷��̾���� �÷��̾ŸƮ ���ͺ��� ������ null�� ��ȯ�ȴ� (����)
	return nullptr;
}
