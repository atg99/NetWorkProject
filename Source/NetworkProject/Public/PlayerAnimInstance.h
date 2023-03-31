// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKPROJECT_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category=AnimSetting)
	float rotAngle;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category=AnimSetting)
	float moveSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AnimSetting)
	bool bIsJumping = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AnimSetting)
	float playerPitch = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AnimSetting)
	float playerYaw = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AnimSetting)
	float playerRoll = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = AnimSetting)
	bool isDead  = false;

	FORCEINLINE bool GetDead(){ return isDead; };

private:
	UPROPERTY()
	class ANetworkPlayer* player;
};
