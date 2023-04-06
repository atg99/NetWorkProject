// Fill out your copyright notice in the Description page of Project Settings.

#include "NetworkPlayer.h"

#include "BattleGameMode.h"
#include "BattlePlayerController.h"
#include "BattlePlayerState.h"
#include "BattleSpectatorPawn.h"
#include "BulletActor.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "MyUserWidget.h"
#include "PlayerAnimInstance.h"
#include "ServerGameInstance.h"
#include "WeaponActor.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/HUD.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ANetworkPlayer::ANetworkPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	playerInfoUI = CreateDefaultSubobject<UWidgetComponent>(TEXT("playerInfoUIII"));
	playerInfoUI->SetupAttachment(GetMesh());
}

void ANetworkPlayer::AutoMove()
{
	if(!bAutoMove)
	{
		bAutoMove = true;
	}
	else
	{
		bAutoMove = false;
	}
}

// Called when the game starts or when spawned
void ANetworkPlayer::BeginPlay()
{
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if(HasAuthority())
	{
		SetHealth(maxHP);
	}

	playerWidget = Cast<UMyUserWidget>(playerInfoUI->GetWidget());
	AnimInstance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	gameInstance = Cast<UServerGameInstance>(GetGameInstance());

	if(GetController() != nullptr && GetController()->IsLocalController())
	{
		SetName(gameInstance->sessionID.ToString());
	}

	/*FTimerHandle NameTimer;
	GetWorldTimerManager().SetTimer(NameTimer, FTimerDelegate::CreateLambda([&](){playerWidget->text_name->SetText(FText::FromString(myName)); }), 1, false);*/
	
}

// Called every frame
void ANetworkPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	playerWidget->text_name->SetText(FText::FromString(myName));
	playerWidget->pb_HP->SetPercent((float)curHP / (float)maxHP);
	if(bIsDead)
	{
		return;
	}
	DrawDebugString(GetWorld(), GetActorLocation(), PrintInfo(), nullptr, FColor::White, 0.0, true, 1.0f);

	if(HasAuthority())
	{
		number++;
		repNumber++;
	}

	if(curHP <= 0)
	{
		DieProcess();
	}

	if(HasAuthority() && bAutoMove)
	{
		AddMovementInput(GetActorForwardVector());
	}
}

FString ANetworkPlayer::PrintInfo()
{
#pragma region ReloInfo
	FString myLocalRole = UEnum::GetValueAsString<ENetRole>(GetLocalRole());
	FString myRemoteRole = UEnum::GetValueAsString<ENetRole>(GetLocalRole());
	FString myConnetion = GetNetConnection() != nullptr ? TEXT("Valid") : TEXT("InValid");
	//플레이어 컨트롤러
	FString myOwner = GetOwner() != nullptr ? GetOwner()->GetName() : TEXT("No Owner");
	FString name = GetName();
	
#pragma endregion

#pragma region RepOrNot
	//FString infoText = FString::Printf(("Number : %d, RepNum : %d"), number, repNumber);

#pragma endregion

	ABattlePlayerController* pc = Cast<ABattlePlayerController>(GetController());

	FString pcString = pc != nullptr ? FString(GetController()->GetName()) : FString("No Controller");
	FString gmString = GetWorld()->GetAuthGameMode() != nullptr ? FString("Has GameMode") : FString("No game mode");
	FString gsString = GetWorld()->GetGameState() != nullptr ? FString("Has GameState") : FString("No Game State");
	FString psString = GetPlayerState() != nullptr ? FString("Has PlayerState") : FString("No PlayerState");
	FString hudStrnig;
	if(pc)
	{
		hudStrnig = pc->GetHUD() != nullptr ? pc->GetHUD()->GetName() : FString("No HUD");
	}

	FString psName;
	if(GetPlayerState())
	{
		psName = GetPlayerState()->GetPlayerName();
	}
	else
	{
		psName = FString("No PlayerState");
	}

	FString gsNames;
	if(GetWorld()->GetGameState())
	{
		for(TObjectPtr<APlayerState> ps : GetWorld()->GetGameState()->PlayerArray)
		{
			gsNames.Append(FString::Printf(TEXT("%s\n"), *ps->GetPlayerName()));
		}
	}

	FString infoText = FString::Printf(TEXT("Local Role: %s\nRemote Role %s\nNet Connection: %s\nOwer: %s\n Name : %s\n%s\n%s\n%s\n%s\n%s\nplayerStateName : %s\n%s"), *myLocalRole, *myRemoteRole, *myConnetion, *myOwner, *GetName(), *pcString, *gmString, *gsString, *psString, *hudStrnig, *psName, *gsNames);

	return infoText;
}

// Called to bind functionality to input
void ANetworkPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANetworkPlayer::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANetworkPlayer::Look);
		EnhancedInputComponent->BindAction(fireAction, ETriggerEvent::Started, this, &ANetworkPlayer::Fire);

		EnhancedInputComponent->BindAction(releaseAction, ETriggerEvent::Started, this, &ANetworkPlayer::ReleaseWeapon);

		EnhancedInputComponent->BindAction(AutoMovingAction, ETriggerEvent::Started, this, &ANetworkPlayer::AutoMove);
	}

}

void ANetworkPlayer::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ANetworkPlayer::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

//서버에서 복제 등록하기 위한 함수
void ANetworkPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//리플리케이트 변수 등록
	//서버에서만 변경해야한다
	DOREPLIFETIME(ANetworkPlayer, curHP);
	DOREPLIFETIME(ANetworkPlayer, ammo);
	DOREPLIFETIME(ANetworkPlayer, bFireDelay);
	DOREPLIFETIME(ANetworkPlayer, myName);

	//조건
	DOREPLIFETIME_CONDITION(ANetworkPlayer, repNumber, COND_OwnerOnly);
	//DOREPLIFETIME_CONDITION(ANetworkPlayer, curHP, COND_OwnerOnly);
	//DOREPLIFETIME_CONDITION(ANetworkPlayer, ammo, COND_OwnerOnly);

}

//서버에 요청하는 함수
void ANetworkPlayer::ServerFire_Implementation()
{
	//총알이 있으면 총알을 생성한다
	if(ammo > 0)
	{
		if(!bFireDelay){
			UE_LOG(LogTemp, Warning, TEXT("Server Fire"));
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Magenta, FString("Server Fire"), true, FVector2D(1.253f));

			ABulletActor* bullet = GetWorld()->SpawnActor<ABulletActor>(bulletFactory, OwningWeapon->meshComp->GetSocketLocation(TEXT("Muzzle")), GetActorRotation());

			bullet->SetOwner(this);
			bullet->attackPower = OwningWeapon->power;

			MultiFire(true);
			ammo--;
			OwningWeapon->ammo--;
			bFireDelay = true;
			FTimerHandle TimerHandle_FireDelay;
			GetWorldTimerManager().SetTimer(TimerHandle_FireDelay, FTimerDelegate::CreateLambda([&](){bFireDelay = false;}), OwningWeapon->reloadTime, false);
		}
	}
	else
	{
		MultiFire(false);
	}
}

//유효한 요청인지 확인한다
bool ANetworkPlayer::ServerFire_Validate()
{
	//return damage > 1000 ? false : true;
	return true;
}

//서버로부터 전달되는 함수
void ANetworkPlayer::MultiFire_Implementation(bool bHasAmmo)
{
	//요청을 받고 모든 클라 및 서버에서 fire를 실행한다
	UE_LOG(LogTemp, Warning, TEXT("Multi Fire"));
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Magenta, FString("Multi Fire"), true, FVector2D(1.253f));
	//GetWorld()->SpawnActor<ABulletActor>(bulletFactory, GetActorLocation()+GetActorForwardVector()*100, GetActorRotation());

	if(bHasAmmo)
	{
		if (fireMontage)
		{
			PlayAnimMontage(fireMontage);
		}
	}
	else
	{
		PlayAnimMontage(NoAmmo);
	}
	
}

void ANetworkPlayer::ClientFire_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Client Fire"));
	GetWorld()->SpawnActor<ABulletActor>(bulletFactory, GetActorLocation() + GetActorForwardVector() * 100, GetActorRotation());
}

void ANetworkPlayer::Fire()
{
	if(!OwningWeapon){ return; }
	UE_LOG(LogTemp, Warning, TEXT("Query Fire"));
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Magenta, FString("Query Fire"), true, FVector2D(1.253f));

	//서버에서 fire를 실행한다 매개변수 사용가능
	ServerFire();
	//ClientFire();

	//TestFunc();
}

//총 내리기
void ANetworkPlayer::ReleaseWeapon()
{
	if(GetController() != nullptr &&GetController()->IsLocalController() == true && OwningWeapon != nullptr)
	{
		OwningWeapon->ServerReleaseWeapon(this);
	}
}

void ANetworkPlayer::SetHealth(int32 value)
{
	curHP = FMath::Min(maxHP, value);
}

void ANetworkPlayer::AddHealth(int32 value)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Magenta, FString("AddHealth"), true, FVector2D(1.253f));
	//curHP = FMath::Clamp(value,curHP, maxHP);
	curHP += value;
}

void ANetworkPlayer::DieProcess()
{
	bIsDead = true;
	//조작을 하는 클라이언트 에서만 실행한다.
	if (GetController())
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("Local DieProcess"));
		playerWidget->pb_HP->SetPercent((float)curHP / (float)maxHP);
		GetCharacterMovement()->DisableMovement();
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		bUseControllerRotationYaw = false;
		FollowCamera->PostProcessSettings.ColorSaturation = FVector4(0, 0, 0, 1);
		ReleaseWeapon();

		GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
	}
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("DieProcess"));
	//게임모드는 서버에만 있으므로 
	//if(HasAuthority())
	//{
	//	//네트워크에서는 지연 이슈로 0.2초 후에 타이머 실행을 권장
	//	
		//ChangeSpectatorMode();


	//	FTimerHandle respawnHandle;
	//	GetWorldTimerManager().SetTimer(respawnHandle, FTimerDelegate::CreateLambda([&]()
	//	{
	//		battleSpectator->GetController()->Possess(this);
	//		battleSpectator->Destroy();
	//		Cast<ABattlePlayerController>(GetController())->Respawn(this);
	//	}), 4.0f, false);
	//}
}

void ANetworkPlayer::ChangeSpectatorMode()
{
	ABattleGameMode* gm = Cast<ABattleGameMode>(GetWorld()->GetAuthGameMode());

	if(gm != nullptr)
	{
		//게임모드에 설정한 관전자 폰 클래스를 불러온다
		TSubclassOf<ASpectatorPawn> spectatorPawn = gm->SpectatorClass;

		FActorSpawnParameters param;
		param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		battleSpectator = GetWorld()->SpawnActor<ABattleSpectatorPawn>(spectatorPawn, GetActorLocation(), GetActorRotation(), param);
		if(battleSpectator != nullptr)
		{
			GetController()->Possess(battleSpectator);
		}
	}
}

void ANetworkPlayer::MultiPlayHitreact_Implementation()
{
	if (curHP <= 0)
	{
		AnimInstance->isDead = true;
	}
	else
	{
		if (hitMontage)
		{
			PlayAnimMontage(hitMontage);
		}
	}
}

void ANetworkPlayer::ServerOnDamage_Implementation(int32 value)
{
	AddHealth(value);
}

void ANetworkPlayer::SetName_Implementation(const FString& name)
{
	//서버에서 이름을 가져오면 안됨
	//myName = Cast<UServerGameInstance>(GetGameInstance())->sessionID.ToString();
	myName = name;

	//서버에서 플레이어 스테이트를 변경한다
	ABattlePlayerState* ps = Cast<ABattlePlayerState>(GetPlayerState());

	if (ps)
	{
		//플레이어이름을 알아서 변경해준다
		ps->SetPlayerName(name);
	}
}

void ANetworkPlayer::EndSession()
{
	if(HasAuthority())
	{
		for(TActorIterator<ANetworkPlayer> pl(GetWorld()); pl; ++pl)
		{
			ANetworkPlayer* p = *pl;
			if(p != this)
			{
				p->MultiDestoryAllSession();
			}
		}
		FTimerHandle TimerHandle_SessionDestroy;
		GetWorldTimerManager().SetTimer(TimerHandle_SessionDestroy, this, &ANetworkPlayer::DestroyMySession, 1, false, 0);
	}
	else
	{
		DestroyMySession();
	}
}

void ANetworkPlayer::DestroyMySession()
{
	//레벨을 처음 위치로 이동시킨다
	gameInstance->sessionInterface->DestroySession(gameInstance->sessionID);

	//세션 ID로 세션을 종료한다
	Cast<ABattlePlayerController>(GetController())->ClientTravel(FString("/Game/Maps/LoginLevel"), ETravelType::TRAVEL_Absolute);
}

void ANetworkPlayer::MultiDestoryAllSession_Implementation()
{
	if(GetController()&&GetController()->IsLocalController())
	{
		//레벨을 처음 위치로 이동시킨다
		gameInstance->sessionInterface->DestroySession(gameInstance->sessionID);

		//세션 ID로 세션을 종료한다
		Cast<ABattlePlayerController>(GetController())->ClientTravel(FString("/Game/Maps/LoginLevel"), ETravelType::TRAVEL_Absolute);
	}
}

void ANetworkPlayer::ServerDestoryAllSession_Implementation()
{
	MultiDestoryAllSession();
}
