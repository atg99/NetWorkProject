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
	APawn* owningPawn = Cast<APawn>(GetOwner());
	//��Ʈ�ѷ��� 0���� �� ������ �� ������ �˸���
	if(owningPawn != nullptr && owningPawn->GetController() != nullptr && owningPawn->GetController()->IsLocalController())
	{
		ServerSpawnEffect();
		ANetworkPlayer* player = Cast<ANetworkPlayer>(OtherActor);
		if(player)
		{
			hitPlayer = player;
			player->playerWidget->SetPlayer(player);
			player->playerWidget->ServerSetHeathBar(10);
		}
		
	}
	//���� �Ѿ��� ��쿡��
	if (HasAuthority())
	{
		ANetworkPlayer* player = Cast<ANetworkPlayer>(OtherActor);
		if (player)
		{
			//ServerHitplayer(player);
			//hitPlayer = player;
			//player->playerWidget->SetPlayer(player);
			//player->playerWidget->ServerSetHeathBar(10);
			player->ServerOnDamage(-10);
			player->MultiPlayHitreact();
		}
	}
}

void ABulletActor::ServerSpawnEffect_Implementation()
{
	//����Ʈ ���
	MultiSpawnParticle();
}

void ABulletActor::MultiSpawnParticle_Implementation()
{
	//����Ʈ ���
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), particle, GetActorLocation());
	//�������� distory�ϸ� ����Ʈ �������� �������� �ȵȴ�
	//Destroy();
}
