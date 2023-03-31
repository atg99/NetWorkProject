// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletActor.generated.h"

UCLASS()
class NETWORKPROJECT_API ABulletActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABulletActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category=BulletSetting)
	class USphereComponent* comp;

	UPROPERTY(EditAnywhere, Category = BulletSetting)
		class UStaticMeshComponent* mesh;

	UPROPERTY(EditAnywhere, Category = BulletSetting)
		float moveSpeed = 300;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
	class UParticleSystem* particle;

	UFUNCTION(NetMulticast, Unreliable)
	void MultiSpawnParticle();

	UFUNCTION(Server, Unreliable)
	void ServerSpawnEffect();

	UPROPERTY()
	class ANetworkPlayer* hitPlayer;
};

