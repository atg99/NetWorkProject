// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleGameStateBase.h"
#include "GameFramework/PlayerState.h"

//�÷��̾� ������Ʈ ����Ʈ�� ������ ���� ������������ �����Ͽ� ��ȯ�ϴ� �Լ�
TArray<APlayerState*> ABattleGameStateBase::GetPlayerListByScore()
{
	TArray<APlayerState*> PlayerList = PlayerArray;

	//���ΰ��� ������?
	PlayerList.Sort([](const APlayerState& a, const APlayerState& b){return a.GetScore() > b.GetScore();});
	return PlayerList;
}
