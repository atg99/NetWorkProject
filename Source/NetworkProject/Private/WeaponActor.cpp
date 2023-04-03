// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponActor.h"

#include "NetworkPlayer.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWeaponActor::AWeaponActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(boxComp);
	boxComp->SetCollisionProfileName(TEXT("WeaponPreset"));
	boxComp->SetSimulatePhysics(true);

	meshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	meshComp->SetupAttachment(boxComp);
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// Called when the game starts or when spawned
void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();

	boxComp->OnComponentBeginOverlap.AddDynamic(this, &AWeaponActor::OnOverlap);
}

// Called every frame
void AWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponActor, ammo);
	DOREPLIFETIME(AWeaponActor, reloadTime);
	DOREPLIFETIME(AWeaponActor, power);
}

void AWeaponActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//서버에서 실행
	if(HasAuthority())
	{
		//플레이어의 무기가 없을 때
		ANetworkPlayer* player = Cast<ANetworkPlayer>(OtherActor);
		if(player&&player->OwningWeapon == nullptr)
		{
			//서버에서만 붙이고 움직임을 복제한다
			//setowner는 서버에서만 가능
			ServerGrapWeapon(player);

			//플레이어의 아모에 나의 아모를 넣는다 
			player->ammo = ammo;
			
		}
	}
}

void AWeaponActor::MulticastReleaseWeapon_Implementation(ANetworkPlayer* player)
{
	//총알을 놓는다
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	boxComp->SetSimulatePhysics(true);
	player->OwningWeapon = nullptr;
	boxComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
}

void AWeaponActor::ServerReleaseWeapon_Implementation(ANetworkPlayer* player)
{
	SetOwner(nullptr);
	//플레이어 총알을 0으로 설정한다
	player->ammo = 0;
	MulticastReleaseWeapon(player);
}

void AWeaponActor::MulticastGrapWeapon_Implementation(class ANetworkPlayer* player)
{
	player->OwningWeapon = this;
	//피직스를 끄고 붙힌다 //모든 클라이언트에 적용되는 부분은 RPC를 사용한다
	boxComp->SetSimulatePhysics(false);
	AttachToComponent(player->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Weapon"));

	FTimerHandle releaseHandle;
	//3초 후에 다시 잡을 수 있게 한다
	GetWorldTimerManager().SetTimer(releaseHandle, FTimerDelegate::CreateLambda([&](){boxComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap); }), 3.0f, false);
	
}

void AWeaponActor::ServerGrapWeapon_Implementation(class ANetworkPlayer* player)
{
	SetOwner(player);
	MulticastGrapWeapon(player);
}
