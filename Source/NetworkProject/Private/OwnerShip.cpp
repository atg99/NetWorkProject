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

	//서버 복제 on/off 함수
	//SetReplicates(true);
	//SetReplicateMovement(true);

}

// Called when the game starts or when spawned
void AOwnerShip::BeginPlay()
{
	Super::BeginPlay();

	//만일 서버 엑터라면
	//if(GetLocalRole() == ROLE_Authority)
	if(HasAuthority()) // 서버에서만 실행한다
	{
		//랜덤한 방향을 설정한다
		moveDirection = FMath::VRand();
		moveDirection.Z = 0;

		FTimerHandle repeatTimer;

		GetWorldTimerManager().SetTimer(repeatTimer, FTimerDelegate::CreateLambda([&](){testNumber = FMath::RandRange(1, 100); /*ScreenLog();*/}), 5.0f, true);
	}

	boxComp->OnComponentBeginOverlap.AddDynamic(this, &AOwnerShip::OnBoxBeginOverlap);
	////랜덤한 방향을 설정한다
	//moveDirection = FMath::VRand();
	//moveDirection.Z = 0;

	//매시의 재질을 다이나믹 머터리얼 인스턴스로 변경한다
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
		//RPC는 onwer의 플레이어 컨트롤러로 받기 때문에 오너가 있어야 RPC 사용가능 대답
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
	//플레이어 컨트롤러
	FString myOwner = GetOwner() != nullptr ? GetOwner()->GetName() : TEXT("No Owner");
	FString name = GetName();
	FString infoText = FString::Printf(TEXT("Local Role: %s\nRemote Role %s\nNet Connection: %s\nOwer: %s\nName : %s"), *myLocalRole, *myRemoteRole, *myConnetion, *myOwner, *name);
#pragma endregion
#pragma region RandomVector
	//서버에만 디렉션 값이 들어가고 클라이언트는 값이 없다
	//FString infoText = FString::Printf(TEXT("%.2f, %.2f, %.2f"), moveDirection.X, moveDirection.Y, moveDirection.Z);
#pragma endregion
	return infoText;
}


//지정된 벙위안에 플레이어가 있다면 가장 가까운 플레이어 캐릭터를 owner로 설정한다.
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

	//RPC는 onwer의 플레이어 컨트롤러로 받기 때문에 오너가 있어야 RPC 사용가능 대답
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
	//"myColor" 파라미터의 값을 matcolor 값으로 변경한다
	mat->SetVectorParameterValue(FName("Color"), (FLinearColor)matColor);
}

//서버에서 복제 등록하기 위한 함수
void AOwnerShip::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//리플리케이트 변수 등록
	//서버에서만 변경해야한다
	DOREPLIFETIME(AOwnerShip, moveDirection);
	DOREPLIFETIME(AOwnerShip, testNumber);
	DOREPLIFETIME(AOwnerShip, matColor);
}

//1. 플레이어 캐릭터가 충돌함면 랜덤한 색상으로 변경
//2, 랜덤 색상은 모든 클라이언트에서 동일하게 보임
