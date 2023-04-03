// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponActor.generated.h"

UCLASS()
class NETWORKPROJECT_API AWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category=MySettings)
	class UBoxComponent* boxComp;

	UPROPERTY(EditAnywhere, Category=MySettings)
	class USkeletalMeshComponent* meshComp;

	UPROPERTY(EditAnywhere, Replicated, Category=MySettings)
	int32 ammo;

	UPROPERTY(EditAnywhere, Replicated, Category=MySettings)
	float reloadTime;

	UPROPERTY(EditAnywhere, Replicated, Category = MySettings)
	float power;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(Server, Unreliable)
	void ServerGrapWeapon(class ANetworkPlayer* player);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastGrapWeapon(class ANetworkPlayer* player);

	UFUNCTION(Server, Unreliable)
	void ServerReleaseWeapon(class ANetworkPlayer* player);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastReleaseWeapon(class ANetworkPlayer* player);
};


