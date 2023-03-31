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

	//�������� ���� �����Ѵ� #include "Net/UnrealNetwork.h"�ʿ�
	UPROPERTY(Replicated)
	FVector moveDirection;

	//���ø�����Ʈ �� ������ ����Ǵ� �Լ� // �� : hp �����̴� �� ���� �Լ� ���� ������ �Լ��� ���� ����
	//�Ű������� ���� ��ȯ �ڷ����� void �� �Լ��� ��밡��
	//���������� ������� �ʴ´�
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


