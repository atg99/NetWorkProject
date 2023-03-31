// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "OwnerShip.generated.h"


UCLASS()
class NETWORKPROJECT_API AOwnerShip : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOwnerShip();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category=MyActor)
	class UStaticMeshComponent* mesh;

	UPROPERTY(EditAnywhere, Category=MyActor)
	class UBoxComponent* boxComp;

	UFUNCTION()
	void ScreenLog();

private:
	FString PrintInfo();

	//서버에서 값을 복제한다 #include "Net/UnrealNetwork.h"필요
	UPROPERTY(Replicated)
	FVector moveDirection;

	//리플리케이트 될 때마다 실행되는 함수 // 예 : hp 슬라이더 바 갱신 함수 변할 때마다 함수도 같이 실행
	//매개변수가 없고 반환 자료형이 void 인 함수만 사용가능
	//서버에서는 실행되지 않는다
	UPROPERTY(ReplicatedUsing = ScreenLog)
	int32 testNumber = 0;

	UPROPERTY(ReplicatedUsing = ChangeColor)
	FVector matColor;

	void CheckOwner();

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//UPROPERTY(EditAnywhere, Category = material)
	//UMaterialInstanceDynamic* myMat;

	UPROPERTY(EditAnywhere, Category = material)
	class UMaterialInstance* myMat;

	UPROPERTY()
	class UMaterialInstanceDynamic* mat;

	UFUNCTION()
	void ChangeColor();
};


