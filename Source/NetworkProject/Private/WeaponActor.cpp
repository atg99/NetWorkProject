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
	//�������� ����
	if(HasAuthority())
	{
		//�÷��̾��� ���Ⱑ ���� ��
		ANetworkPlayer* player = Cast<ANetworkPlayer>(OtherActor);
		if(player&&player->OwningWeapon == nullptr)
		{
			//���������� ���̰� �������� �����Ѵ�
			//setowner�� ���������� ����
			ServerGrapWeapon(player);

			//�÷��̾��� �Ƹ� ���� �Ƹ� �ִ´� 
			player->ammo = ammo;
			
		}
	}
}

void AWeaponActor::MulticastReleaseWeapon_Implementation(ANetworkPlayer* player)
{
	//�Ѿ��� ���´�
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	boxComp->SetSimulatePhysics(true);
	player->OwningWeapon = nullptr;
	boxComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
}

void AWeaponActor::ServerReleaseWeapon_Implementation(ANetworkPlayer* player)
{
	SetOwner(nullptr);
	//�÷��̾� �Ѿ��� 0���� �����Ѵ�
	player->ammo = 0;
	MulticastReleaseWeapon(player);
}

void AWeaponActor::MulticastGrapWeapon_Implementation(class ANetworkPlayer* player)
{
	player->OwningWeapon = this;
	//�������� ���� ������ //��� Ŭ���̾�Ʈ�� ����Ǵ� �κ��� RPC�� ����Ѵ�
	boxComp->SetSimulatePhysics(false);
	AttachToComponent(player->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Weapon"));

	FTimerHandle releaseHandle;
	//3�� �Ŀ� �ٽ� ���� �� �ְ� �Ѵ�
	GetWorldTimerManager().SetTimer(releaseHandle, FTimerDelegate::CreateLambda([&](){boxComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap); }), 3.0f, false);
	
}

void AWeaponActor::ServerGrapWeapon_Implementation(class ANetworkPlayer* player)
{
	SetOwner(player);
	MulticastGrapWeapon(player);
}
