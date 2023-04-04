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

	//미리보기에서는 플레이어가 없으므로 예외처리
	if(player == nullptr){ return; }

	moveSpeed = player->GetVelocity().Length();
#pragma region Calculate Angle
	//FVector forwordVec = player->GetActorForwardVector();
	FVector forwordVec = FRotationMatrix(player->GetActorRotation()).GetUnitAxis(EAxis::X); //(EAxis::X)

	FVector normalVelocity = player->GetVelocity().GetSafeNormal2D();

	//회전 각도 계산
	double cosTheta = FVector::DotProduct(forwordVec, normalVelocity);
	double rad = FMath::Acos(cosTheta);
	float degree = FMath::RadiansToDegrees(rad);

	FVector rightVec = FRotationMatrix(player->GetActorRotation()).GetUnitAxis(EAxis::Y);

	//회전 방향 계산
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

	//죽었는지 확인
	//isDead = player->bIsDead;
}

//서버에서 복제 등록하기 위한 함수
void UPlayerAnimInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//리플리케이트 변수 등록
	//서버에서만 변경해야한다
	//DOREPLIFETIME(ANetworkPlayer, curHP);
	DOREPLIFETIME(UPlayerAnimInstance, isDead);

	//조건
	//DOREPLIFETIME_CONDITION(ANetworkPlayer, repNumber, COND_OwnerOnly);
	//DOREPLIFETIME_CONDITION(ANetworkPlayer, curHP, COND_OwnerOnly);
	//DOREPLIFETIME_CONDITION(ANetworkPlayer, ammo, COND_OwnerOnly);
}