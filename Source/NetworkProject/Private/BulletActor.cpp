// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletActor.h"

#include "MyUserWidget.h"
#include "NetworkPlayer.h"
#include "Components/SphereComponent.h"
#include "Engine/StaticMeshSocket.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABulletActor::ABulletActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	comp = CreateDefaultSubobject<USphereComponent>(TEXT("comp"));
	SetRootComponent(comp);
	comp->SetSphereRadius(15.f);
	comp->SetCollisionProfileName(TEXT("BulletPreset"));

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	mesh->SetupAttachment(RootComponent);
	mesh->SetWorldScale3D(FVector(0.3));
	mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ABulletActor::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(10);
	comp->OnComponentBeginOverlap.AddDynamic(this, &ABulletActor::OnOverlap);
}

// Called every frame
void ABulletActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorLocation(GetActorLocation()+GetActorForwardVector()*moveSpeed*DeltaTime);
}

void ABulletActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(GetOwner() == nullptr)
	{
		return;
	}
	//서버 총알인 경우에만
	if (HasAuthority())
	{
		ANetworkPlayer* player = Cast<ANetworkPlayer>(OtherActor);
		// 오너가 설정되기 전에는 데미지를 실행하지 않음
		if (player && player != GetOwner())
		{
			player->ServerOnDamage(-attackPower);
			player->MultiPlayHitreact();
			Destroy();
		}
	}
}

void ABulletActor::Destroyed()
{
	Super::Destroyed();

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), particle, GetActorLocation());
}
