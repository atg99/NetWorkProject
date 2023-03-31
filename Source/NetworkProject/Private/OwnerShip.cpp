// Fill out your copyright notice in the Description page of Project Settings.


#include "OwnerShip.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "NetworkPlayer.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AOwnerShip::AOwnerShip()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxComp  = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	SetRootComponent(boxComp);
	boxComp->SetBoxExtent(FVector(50));

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	mesh->SetupAttachment(RootComponent);
	mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//���� ���� on/off �Լ�
	//SetReplicates(true);
	//SetReplicateMovement(true);

}

// Called when the game starts or when spawned
void AOwnerShip::BeginPlay()
{
	Super::BeginPlay();

	//���� ���� ���Ͷ��
	//if(GetLocalRole() == ROLE_Authority)
	if(HasAuthority()) // ���������� �����Ѵ�
	{
		//������ ������ �����Ѵ�
		moveDirection = FMath::VRand();
		moveDirection.Z = 0;

		FTimerHandle repeatTimer;

		GetWorldTimerManager().SetTimer(repeatTimer, FTimerDelegate::CreateLambda([&](){testNumber = FMath::RandRange(1, 100); /*ScreenLog();*/}), 5.0f, true);
	}

	boxComp->OnComponentBeginOverlap.AddDynamic(this, &AOwnerShip::OnBoxBeginOverlap);
	////������ ������ �����Ѵ�
	//moveDirection = FMath::VRand();
	//moveDirection.Z = 0;

	//�Ž��� ������ ���̳��� ���͸��� �ν��Ͻ��� �����Ѵ�
	UMaterialInterface* base_mat = mesh->GetMaterial(0);
	if(base_mat)
	{
		mat = UMaterialInstanceDynamic::Create(base_mat, this);
		mesh->SetMaterial(0, mat);
	}
}

// Called every frame
void AOwnerShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PrintInfo();

	DrawDebugString(GetWorld(), GetActorLocation(), PrintInfo(), nullptr, FColor::White, 0.0, true, 1.0f);

	//SetActorLocation(GetActorLocation() + moveDirection * 100* DeltaTime, true);

	if(HasAuthority())
	{
		//RPC�� onwer�� �÷��̾� ��Ʈ�ѷ��� �ޱ� ������ ���ʰ� �־�� RPC ��밡�� ���
		CheckOwner();
	}

	DrawDebugSphere(GetWorld(), GetActorLocation(), 500.f, 30, FColor::Cyan, false, 0.0f, 0, 1);
}

void AOwnerShip::ScreenLog()
{
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::FromInt(testNumber), true, FVector2D(1.5f));
	}
}

FString AOwnerShip::PrintInfo()
{
#pragma region ReloInfo
	FString myLocalRole = UEnum::GetValueAsString<ENetRole>(GetLocalRole());
	FString myRemoteRole = UEnum::GetValueAsString<ENetRole>(GetLocalRole());
	FString myConnetion = GetNetConnection() != nullptr ? TEXT("Valid") : TEXT("InValid");
	//�÷��̾� ��Ʈ�ѷ�
	FString myOwner = GetOwner() != nullptr ? GetOwner()->GetName() : TEXT("No Owner");
	FString name = GetName();
	FString infoText = FString::Printf(TEXT("Local Role: %s\nRemote Role %s\nNet Connection: %s\nOwer: %s\nName : %s"), *myLocalRole, *myRemoteRole, *myConnetion, *myOwner, *name);
#pragma endregion
#pragma region RandomVector
	//�������� �𷺼� ���� ���� Ŭ���̾�Ʈ�� ���� ����
	//FString infoText = FString::Printf(TEXT("%.2f, %.2f, %.2f"), moveDirection.X, moveDirection.Y, moveDirection.Z);
#pragma endregion
	return infoText;
}


//������ �����ȿ� �÷��̾ �ִٸ� ���� ����� �÷��̾� ĳ���͸� owner�� �����Ѵ�.
void AOwnerShip::CheckOwner()
{
	float maxDistance = 500;
	ANetworkPlayer* player = nullptr;

	TArray<AActor*> Players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANetworkPlayer::StaticClass(), Players);
	float dis = 500;
	for (auto Player : Players)
	{
		if(GetDistanceTo(Player) <= dis)
		{
			dis = GetDistanceTo(Player);
			player = Cast<ANetworkPlayer>(Player);
		}
	}

	if(player && GetOwner() != player)
	{
		SetOwner(player);
	}

	/*for(TActorIterator<ANetworkPlayer> it(GetWorld()); it; ++it)
	{
		float distance = this->GetDistanceTo(*it);

		if(distance < maxDistance)
		{
			maxDistance = distance;
			player = *it;
		}
	}

	if(player && GetOwner()!=player)
	{
		SetOwner(player);
	}*/

	//RPC�� onwer�� �÷��̾� ��Ʈ�ѷ��� �ޱ� ������ ���ʰ� �־�� RPC ��밡�� ���
}

void AOwnerShip::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(HasAuthority())
	{
		if (OtherActor->IsA<ANetworkPlayer>())
		//if(OtherActor->IsA(ANetworkPlayer::StaticClass()))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow)
			matColor = FMath::VRand();
			matColor = matColor.GetAbs();
			ChangeColor();
		}
	}
}

void AOwnerShip::ChangeColor()
{
	//"myColor" �Ķ������ ���� matcolor ������ �����Ѵ�
	mat->SetVectorParameterValue(FName("Color"), (FLinearColor)matColor);
}

//�������� ���� ����ϱ� ���� �Լ�
void AOwnerShip::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//���ø�����Ʈ ���� ���
	//���������� �����ؾ��Ѵ�
	DOREPLIFETIME(AOwnerShip, moveDirection);
	DOREPLIFETIME(AOwnerShip, testNumber);
	DOREPLIFETIME(AOwnerShip, matColor);
}

//1. �÷��̾� ĳ���Ͱ� �浹�Ը� ������ �������� ����
//2, ���� ������ ��� Ŭ���̾�Ʈ���� �����ϰ� ����
