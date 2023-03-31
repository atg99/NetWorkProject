// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"
#include "NetworkPlayer.generated.h"


UCLASS()
class ANetworkPlayer : public ACharacter
{
	GENERATED_BODY()

		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* playerInfoUI;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* fireAction;
public:
	ANetworkPlayer();


protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:
	FString PrintInfo();

	UPROPERTY()
	int32 number;
	
	UPROPERTY(Replicated)
	int32 repNumber;

public:

	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerFire();

	UFUNCTION(Client, Unreliable)
	void ClientFire();

	UFUNCTION(NetMulticast, Unreliable)
	void MultiFire();

	void Fire();


	UPROPERTY(EditDefaultsOnly, Category = MySet)
		TSubclassOf<class ABulletActor> bulletFactory;

	UPROPERTY(EditAnywhere, Category = Anim)
	class UAnimMontage* fireMontage;


	UPROPERTY(EditAnywhere, Category = Anim)
		class UAnimMontage* hitMontage;

	UPROPERTY(EditAnywhere, Replicated)
	int32 curHP;

	UPROPERTY(EditAnywhere)
	int32 maxHP = 100;

	UPROPERTY(EditAnywhere, Replicated)
	int32 ammo;

	UFUNCTION()
	void SetHealth(int32 value);

	UFUNCTION()
	void AddHealth(int32 value);

	UFUNCTION(Server, Unreliable)
	void ServerOnDamage(int32 value);

	UFUNCTION(NetMulticast, Unreliable)
	void MultiPlayHitreact();

	FORCEINLINE int32 GetHealth(){ return curHP;}
	FORCEINLINE int32 GetAmmo(){ return ammo;}

	UPROPERTY(VisibleAnywhere)
	class UMyUserWidget* playerWidget;

	UPROPERTY()
	class UPlayerAnimInstance* AnimInstance;

	UPROPERTY()
	bool bIsDead = false;
};
