// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "NetworkPlayer.h"
#include "KismetAnimationLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	player = Cast<ANetworkPlayer>(GetOwningActor());
	//player->AnimInstance = this;
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	//�̸����⿡���� �÷��̾ �����Ƿ� ����ó��
	if(player == nullptr){ return; }

	moveSpeed = player->GetVelocity().Length();
#pragma region Calculate Angle
	//FVector forwordVec = player->GetActorForwardVector();
	FVector forwordVec = FRotationMatrix(player->GetActorRotation()).GetUnitAxis(EAxis::X); //(EAxis::X)

	FVector normalVelocity = player->GetVelocity().GetSafeNormal2D();

	//ȸ�� ���� ���
	double cosTheta = FVector::DotProduct(forwordVec, normalVelocity);
	double rad = FMath::Acos(cosTheta);
	float degree = FMath::RadiansToDegrees(rad);

	FVector rightVec = FRotationMatrix(player->GetActorRotation()).GetUnitAxis(EAxis::Y);

	//ȸ�� ���� ���
	cosTheta = FVector::DotProduct(rightVec, normalVelocity);

	rotAngle = cosTheta >= 0 ? degree : degree * -1.0f;
#pragma endregion
	//rotAngle =  CalculateDirection(player->GetVelocity(), player->GetActorRotation());
	//rotAngle = UKismetAnimationLibrary::CalculateDirection(player->GetVelocity(), player->GetActorRotation());

	bIsJumping = player->GetCharacterMovement()->IsFalling();

	player->GetBaseAimRotation();

	FRotator viewRot = player->GetBaseAimRotation();
	FRotator playerRot = player->GetActorRotation();
	FRotator deltaRot = playerRot - viewRot;
	playerPitch = FMath::Clamp(deltaRot.GetNormalized().Pitch, -45.f, 45.f);
	playerYaw = deltaRot.GetNormalized().Yaw;
	playerRoll = deltaRot.GetNormalized().Roll;

	//�׾����� Ȯ��
	//isDead = player->bIsDead;
}

//�������� ���� ����ϱ� ���� �Լ�
void UPlayerAnimInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//���ø�����Ʈ ���� ���
	//���������� �����ؾ��Ѵ�
	//DOREPLIFETIME(ANetworkPlayer, curHP);
	DOREPLIFETIME(UPlayerAnimInstance, isDead);

	//����
	//DOREPLIFETIME_CONDITION(ANetworkPlayer, repNumber, COND_OwnerOnly);
	//DOREPLIFETIME_CONDITION(ANetworkPlayer, curHP, COND_OwnerOnly);
	//DOREPLIFETIME_CONDITION(ANetworkPlayer, ammo, COND_OwnerOnly);
}