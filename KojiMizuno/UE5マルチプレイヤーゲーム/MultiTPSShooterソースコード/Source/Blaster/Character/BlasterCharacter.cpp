// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/BlasterComponents/BuffComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "BlasterAnimInstance.h"
#include "Blaster/Blaster.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Components/BoxComponent.h"
#include "Blaster/BlasterComponents/LagCompensationComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Blaster/PlayerStart/TeamPlayerStart.h"

ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Rotate Only Camera
	bUseControllerRotationYaw = false;
	// Character Face WASD key direction if true
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Overheadwidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	//Combat2 = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat2Component"));
	Combat2 = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat2Component"));
	Combat2->SetIsReplicated(true);

	Buff = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	Buff->SetIsReplicated(true);

	LagCompensation = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensation"));
	//LagCompensation->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetCharacterMovement()->RotationRate = FRotator(0.f, 850.f, 0.f);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));

	AttachedGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Attached Grenade"));
	AttachedGrenade->SetupAttachment(GetMesh(), FName("GrenadeSocket"));
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/* 5.1で必要になる可能性
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	*/


	/**
	* Hit boxes for server-side rewind
	*/

	head = CreateDefaultSubobject<UBoxComponent>(TEXT("head"));
	head->SetupAttachment(GetMesh(), FName("head"));
	HitCollisionBoxes.Add(FName("head"), head);

	pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("pelvis"));
	pelvis->SetupAttachment(GetMesh(), FName("pelvis"));
	HitCollisionBoxes.Add(FName("pelvis"), pelvis);

	spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_02"));
	spine_02->SetupAttachment(GetMesh(), FName("spine_02"));
	HitCollisionBoxes.Add(FName("spine_02"), spine_02);

	spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_03"));
	spine_03->SetupAttachment(GetMesh(), FName("spine_03"));
	HitCollisionBoxes.Add(FName("spine_03"), spine_03);

	upperarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l"));
	upperarm_l->SetupAttachment(GetMesh(), FName("upperarm_l"));
	HitCollisionBoxes.Add(FName("upperarm_l"), upperarm_l);

	upperarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r"));
	upperarm_r->SetupAttachment(GetMesh(), FName("upperarm_r"));
	HitCollisionBoxes.Add(FName("upperarm_r"), upperarm_r);

	lowerarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
	lowerarm_l->SetupAttachment(GetMesh(), FName("lowerarm_l"));
	HitCollisionBoxes.Add(FName("lowerarm_l"), lowerarm_l);

	lowerarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
	lowerarm_r->SetupAttachment(GetMesh(), FName("lowerarm_r"));
	HitCollisionBoxes.Add(FName("lowerarm_r"), lowerarm_r);

	hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_l"));
	hand_l->SetupAttachment(GetMesh(), FName("hand_l"));
	HitCollisionBoxes.Add(FName("hand_l"), hand_l);

	hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_r"));
	hand_r->SetupAttachment(GetMesh(), FName("hand_r"));
	HitCollisionBoxes.Add(FName("hand_r"), hand_r);

	blanket = CreateDefaultSubobject<UBoxComponent>(TEXT("blanket"));
	blanket->SetupAttachment(GetMesh(), FName("backpack"));
	HitCollisionBoxes.Add(FName("blanket"), blanket);

	backpack = CreateDefaultSubobject<UBoxComponent>(TEXT("backpack"));
	backpack->SetupAttachment(GetMesh(), FName("backpack"));
	HitCollisionBoxes.Add(FName("backpack"), backpack);

	thigh_l = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_l"));
	thigh_l->SetupAttachment(GetMesh(), FName("thigh_l"));
	HitCollisionBoxes.Add(FName("thigh_l"), thigh_l);

	thigh_r = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_r"));
	thigh_r->SetupAttachment(GetMesh(), FName("thigh_r"));
	HitCollisionBoxes.Add(FName("thigh_r"), thigh_r);

	calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_l"));
	calf_l->SetupAttachment(GetMesh(), FName("calf_l"));
	HitCollisionBoxes.Add(FName("calf_l"), calf_l);

	calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_r"));
	calf_r->SetupAttachment(GetMesh(), FName("calf_r"));
	HitCollisionBoxes.Add(FName("calf_r"), calf_r);

	foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_l"));
	foot_l->SetupAttachment(GetMesh(), FName("foot_l"));
	HitCollisionBoxes.Add(FName("foot_l"), foot_l);

	foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_r"));
	foot_r->SetupAttachment(GetMesh(), FName("foot_r"));
	HitCollisionBoxes.Add(FName("foot_r"), foot_r);

	for (auto Box : HitCollisionBoxes)
	{
		if (Box.Value)
		{
			Box.Value->SetCollisionObjectType(ECC_HitBox);
			Box.Value->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			Box.Value->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
			Box.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ABlasterCharacter, Health);
	DOREPLIFETIME(ABlasterCharacter, Shield);
	DOREPLIFETIME(ABlasterCharacter, bDisableGameplay);
	//DOREPLIFETIME(ABlasterCharacter, CharacterTeam);
}

void ABlasterCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();

	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

/*
* 死亡時呼び出される
*/
void ABlasterCharacter::Elim(bool bPlayerLeftGame)
{
	DropOrDestroyWeapons();
	MulticastElim(bPlayerLeftGame);
}

void ABlasterCharacter::MulticastElim_Implementation(bool bPlayerLeftGame)
{
	bLeftGame = bPlayerLeftGame;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDWeaponAmmo(0);
	}

	bElimmed = true;
	PlayElimMontage();

	// Start dissolve effect(死亡時のエフェクト)
	if (DissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), 0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Grow"), 200.f);
	}
	StartDissolve();

	// Disable Character movement
	/*
	GetCharacterMovement()->DisableMovement(); // disable awsd key
	GetCharacterMovement()->StopMovementImmediately(); // disable character rotate
	if (BlasterPlayerController)
	{
		DisableInput(BlasterPlayerController); // disable all input
	}*/
	bDisableGameplay = true;
	GetCharacterMovement()->DisableMovement(); // 床下に落下していくのを防止
	if (Combat2)
	{
		Combat2->FireButtonPressed(false);
	}
	// Disable collision(コリジョンオフ)
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Spawn elim bot(ドローンスポーン)
	if (ElimBotEffect)
	{
		FVector ElimBotSpawnPoint(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 200.f);
		ElimBotComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ElimBotEffect, ElimBotSpawnPoint, GetActorRotation());
	}
	if (ElimBotSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, ElimBotSound, GetActorLocation());
	}

	// スナイパーのスコープを覗いていたらオフにする
	bool bHideSniperScope = IsLocallyControlled() &&
		Combat2 &&
		Combat2->bAiming &&
		Combat2->EquippedWeapon &&
		Combat2->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle;
	if (bHideSniperScope)
	{
		ShowSniperScopeWidget(false);
	}

	if (CrownComponent)
	{
		CrownComponent->DestroyComponent();
	}

	GetWorldTimerManager().SetTimer(ElimTimer, this, &ABlasterCharacter::ElimTimerFinished, ElimDelay);
}

/*
* 死亡後一定期間後に呼び出される
*/
void ABlasterCharacter::ElimTimerFinished()
{
	BlasterGameMode = BlasterGameMode == nullptr ? GetWorld()->GetAuthGameMode<ABlasterGameMode>() : BlasterGameMode;
	if (BlasterGameMode && !bLeftGame)
	{
		BlasterGameMode->RequestRespawn(this, Controller);
	}
	if (bLeftGame && IsLocallyControlled())
	{
		OnLeftGame.Broadcast();
	}
}

/*
* サーバー側がゲームを離れたとき
*/
void ABlasterCharacter::ServerLeaveGame_Implementation()
{
	BlasterGameMode = BlasterGameMode == nullptr ? GetWorld()->GetAuthGameMode<ABlasterGameMode>() : BlasterGameMode;
	BlasterPlayerState = BlasterPlayerState == nullptr ? GetPlayerState<ABlasterPlayerState>() : BlasterPlayerState;
	if (BlasterGameMode && BlasterPlayerState)
	{
		BlasterGameMode->PlayerLeftGame(BlasterPlayerState);
	}
}

void ABlasterCharacter::DropOrDestroyWeapon(AWeapon* Weapon)
{
	if (Weapon == nullptr) return;
	if (Weapon->bDestroyWeapon)
	{
		Weapon->Destroy();
	}
	else
	{
		Weapon->Dropped();
	}
}

void ABlasterCharacter::DropOrDestroyWeapons()
{
	if (Combat2)
	{
		if (Combat2->EquippedWeapon)
		{
			DropOrDestroyWeapon(Combat2->EquippedWeapon);
		}
		if (Combat2->SecondaryWeapon)
		{
			DropOrDestroyWeapon(Combat2->SecondaryWeapon);
		}
		if (Combat2->TheFlag)
		{
			Combat2->TheFlag->Dropped();
		}
	}
}

/*
* プレイヤーステートの初期化
* チームカラーの設定
* スポーンポイントの設定
*/
void ABlasterCharacter::OnPlayerStateInitialized()
{
	BlasterPlayerState->AddToScore(0.f);
	BlasterPlayerState->AddToDefeats(0);
	SetTeamColor(BlasterPlayerState->GetTeam());
	SetSpawnPoint();
}

void ABlasterCharacter::SetSpawnPoint()
{
	if (HasAuthority() && BlasterPlayerState->GetTeam() != ETeam::ET_NoTeam)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, ATeamPlayerStart::StaticClass(), PlayerStarts);
		TArray<ATeamPlayerStart*> TeamPlayerStarts;
		for (auto Start : PlayerStarts)
		{
			ATeamPlayerStart* TeamStart = Cast<ATeamPlayerStart>(Start);
			if (TeamStart && TeamStart->Team == BlasterPlayerState->GetTeam())
			{
				TeamPlayerStarts.Add(TeamStart);
			}
		}
		if (TeamPlayerStarts.Num() > 0)
		{
			ATeamPlayerStart* ChosenPlayerStart = TeamPlayerStarts[FMath::RandRange(0, TeamPlayerStarts.Num() - 1)];
			SetActorLocationAndRotation(
				ChosenPlayerStart->GetActorLocation(),
				ChosenPlayerStart->GetActorRotation()
			);
		}
	}
}

/*
* ドローンと武器を消す
*/
void ABlasterCharacter::Destroyed()
{
	if (ElimBotComponent)
	{
		ElimBotComponent->DestroyComponent();
	}
	BlasterGameMode = BlasterGameMode == nullptr ? GetWorld()->GetAuthGameMode<ABlasterGameMode>() : BlasterGameMode;
	bool bMatchNotInProgress = BlasterGameMode && BlasterGameMode->GetMatchState() != MatchState::InProgress;
	if (Combat2 && Combat2->EquippedWeapon && bMatchNotInProgress)
	{
		Combat2->EquippedWeapon->Destroy();
	}
}

/*
* キルリーダーのエフェクト表示
*/
void ABlasterCharacter::MulticastGainedTheLead_Implementation()
{
	if (CrownSystem == nullptr) return;
	if (CrownComponent == nullptr)
	{
		CrownComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			CrownSystem,
			GetMesh(),
			FName(),
			GetActorLocation() + FVector(0.f, 0.f, 110.f),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false
		);
	}
	if (CrownComponent)
	{
		CrownComponent->Activate();
	}
}

void ABlasterCharacter::MulticastLostTheLead_Implementation()
{
	if (CrownComponent)
	{
		CrownComponent->DestroyComponent();
	}
}

void ABlasterCharacter::SetTeamColor(ETeam Team)
{
	//CharacterTeam = Team;
	if (GetMesh() == nullptr || OriginalMaterial == nullptr) return;
	switch (Team)
	{
	case ETeam::ET_NoTeam:
		GetMesh()->SetMaterial(0, OriginalMaterial);
		DissolveMaterialInstance = BlueDissolveMatInst;
		break;
	case ETeam::ET_BlueTeam:
		GetMesh()->SetMaterial(0, BlueMaterial);
		DissolveMaterialInstance = BlueDissolveMatInst;
		break;
	case ETeam::ET_RedTeam:
		GetMesh()->SetMaterial(0, RedMaterial);
		DissolveMaterialInstance = RedDissolveMatInst;
		break;
	}
}

/*
void ABlasterCharacter::OnRep_Team()
{
	//ABlasterPlayerState* BlasterPlayerState2 = GetPlayerState<ABlasterPlayerState>();
	if (GetMesh() == nullptr || OriginalMaterial == nullptr) return;
	switch (CharacterTeam)
	{
	case ETeam::ET_NoTeam:
		GetMesh()->SetMaterial(0, OriginalMaterial);
		DissolveMaterialInstance = BlueDissolveMatInst;
		break;
	case ETeam::ET_BlueTeam:
		GetMesh()->SetMaterial(0, BlueMaterial);
		DissolveMaterialInstance = BlueDissolveMatInst;
		break;
	case ETeam::ET_RedTeam:
		GetMesh()->SetMaterial(0, RedMaterial);
		DissolveMaterialInstance = RedDissolveMatInst;
		UE_LOG(LogTemp, Warning, TEXT("OnRep_Team"));
		break;
	}
}*/

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	SpawDefaultWeapon();
	UpdateHUDAmmo();
	UpdateHUDHealth();
	UpdateHUDShield();
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ABlasterCharacter::ReceiveDamage);
	}
	if (AttachedGrenade)
	{
		AttachedGrenade->SetVisibility(false);
	}
	
}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateInPlace(DeltaTime);
	HideCameraIfCharacterClose();
	PollInit();

	
}

/*
* 視点移動時のキャラクターアニメーション
*/
void ABlasterCharacter::RotateInPlace(float DeltaTime)
{
	if (bDisableGameplay)
	{
		if (Combat2 && Combat2->bHoldingTheFlag)
		{
			bUseControllerRotationYaw = false;
			GetCharacterMovement()->bOrientRotationToMovement = true;
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			return;
		}
		if (Combat2 && Combat2->EquippedWeapon) GetCharacterMovement()->bOrientRotationToMovement = false;
		if (Combat2 && Combat2->EquippedWeapon) bUseControllerRotationYaw = true;
		if (bDisableGameplay)
		{
			bUseControllerRotationYaw = false;
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			return;
		}
		return;
	}
	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABlasterCharacter::Jump);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ABlasterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ABlasterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &ABlasterCharacter::LookUp);
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &ABlasterCharacter::Turn);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ABlasterCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABlasterCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABlasterCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABlasterCharacter::AimButtonReleased);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABlasterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ABlasterCharacter::FireButtonReleased);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ABlasterCharacter::ReloadButtonPressed);
	PlayerInputComponent->BindAction("ThrowGrenade", IE_Pressed, this, &ABlasterCharacter::GrenadeButtonPressed);
}

/*
* 各コンポーネント変数の設定
*/
void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents(); /* コンストラクタの後に呼び出される(https://docs.unrealengine.com/5.2/en-US/unreal-engine-actor-lifecycle/)*/
	if (Combat2)
	{
		Combat2->Character = this;
	}
	if (Buff)
	{
		Buff->Character = this;
		Buff->SetInitialSpeeds(
			GetCharacterMovement()->MaxWalkSpeed,
			GetCharacterMovement()->MaxWalkSpeedCrouched
		);
		Buff->SetInitialJumpVelocity(GetCharacterMovement()->JumpZVelocity);
	}
	if (LagCompensation)
	{
		LagCompensation->Character = this;
		if (Controller)
		{
			LagCompensation->Controller = Cast<ABlasterPlayerController>(Controller);
		}
	}
}

void ABlasterCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat2 == nullptr || Combat2->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayReloadMontage()
{
	if (Combat2 == nullptr || Combat2->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	FOnMontageEnded CompleteDelegate;
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;
		
		switch (Combat2->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_RocketLauncher:
			SectionName = FName("RocketLauncher");
			break;
		case EWeaponType::EWT_Pistol:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_SubmachineGun:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_Shotgun:
			SectionName = FName("Shotgun");
			break;
		case EWeaponType::EWT_SniperRifle:
			SectionName = FName("SniperRifle");
			break;
		case EWeaponType::EWT_GrenadeLauncher:
			SectionName = FName("GrenadeLauncher");
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName);

		// モンタージュの再生中に途切れた場合の処理(主にリロード中に被撃時、ステートがリロードのままになることを防ぐ)
		// UAnimInstance::TriggerMontageEndedEvent
		CompleteDelegate.BindWeakLambda(this, [this](UAnimMontage* animMontage, bool bInterrupted)->void
			{
				if (bInterrupted)
				{
					UE_LOG(LogTemp, Warning, TEXT("We were interrupted"));
					// We didnt finish playing this animation
					Combat2->IntrupttedFinishReloading();
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("We completed"));
					// animation finished
				}
			});
	}
	AnimInstance->Montage_SetEndDelegate(CompleteDelegate, ReloadMontage);
}

void ABlasterCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
		/*
		if (!HasAuthority() && !IsLocallyControlled())
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), "yes");
		}*/
	}
}

void ABlasterCharacter::PlayThrowGrenadeMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ThrowGrenadeMontage)
	{
		AnimInstance->Montage_Play(ThrowGrenadeMontage);
	}
}

void ABlasterCharacter::PlaySwapMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && SwapMontage)
	{
		AnimInstance->Montage_Play(SwapMontage);
	}
}


void ABlasterCharacter::PlayHitReactMontage()
{
	if (Combat2 == nullptr || Combat2->EquippedWeapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("FromFront");
		AnimInstance->Montage_JumpToSection(SectionName);
		//UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), "yes");
	}
}

void ABlasterCharacter::GrenadeButtonPressed()
{
	if (Combat2 && Combat2->bHoldingTheFlag) return;
	if (Combat2)
	{
		Combat2->ThrowGrenade();
	}
}

/*
* ダメージ処理
*/
void ABlasterCharacter::ReceiveDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser)
{
	BlasterGameMode = BlasterGameMode == nullptr ? GetWorld()->GetAuthGameMode<ABlasterGameMode>() : BlasterGameMode;
	if (bElimmed || BlasterGameMode == nullptr) return;
	Damage = BlasterGameMode->CalculateDamage(InstigatorController, Controller, Damage);
	float DamageToHealth = Damage;
	if (Shield > 0.f)
	{
		if (Shield >= Damage)
		{
			Shield = FMath::Clamp(Shield - Damage, 0.f, MaxShield);
			DamageToHealth = 0.f;
		}
		else
		{
			DamageToHealth = FMath::Clamp(DamageToHealth - Shield, 0.f, Damage);
			Shield = 0.f;
		}
	}

	Health = FMath::Clamp(Health - DamageToHealth, 0.f, MaxHealth);

	PlayHitReactMontage();
	UpdateHUDHealth();
	UpdateHUDShield();

	if (Health == 0.f)
	{
		if (BlasterGameMode)
		{
			BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
			ABlasterPlayerController* AttackerController = Cast<ABlasterPlayerController>(InstigatorController); // Change From ABlasterController To AController for PlayerEliminated input
			BlasterGameMode->PlayerEliminated(this, BlasterPlayerController, AttackerController);
		}
	}
}

void ABlasterCharacter::MoveForward(float Value)
{
	if (bDisableGameplay) return;
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation{ 0.f, Controller->GetControlRotation().Yaw, 0.f };
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value);
	}
}

void ABlasterCharacter::MoveRight(float Value)
{
	if (bDisableGameplay) return;
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation{ 0.f, Controller->GetControlRotation().Yaw, 0.f };
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, Value);
	}
}

void ABlasterCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ABlasterCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

/*
* 拾うボタン押下時(EKey)
*/
void ABlasterCharacter::EquipButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat2)
	{
		if (Combat2->bHoldingTheFlag) return;
		if (Combat2->CombatState == ECombatState::ECS_Unoccupied) ServerEquipButtonPressed();
		bool bSwap = Combat2->ShouldSwapWeapons() &&
			!HasAuthority() &&
			Combat2->CombatState == ECombatState::ECS_Unoccupied &&
			OverlappingWeapon == nullptr;

		if (bSwap)
		{
			PlaySwapMontage();
			Combat2->CombatState = ECombatState::ECS_SwappingWeapons;
			bFinishedSwapping = false;
		}
	}
}

void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat2)
	{
		if (OverlappingWeapon)
		{
			Combat2->EquipWeapon(OverlappingWeapon);
		}
		else if (Combat2->ShouldSwapWeapons())
		{
			Combat2->SwapWeapons();
		}
	}
}

void ABlasterCharacter::CrouchButtonPressed()
{
	if (Combat2 && Combat2->bHoldingTheFlag) return;
	if (bDisableGameplay) return;
	if (bIsCrouched)
	{
		UnCrouch(); // ACharacter->UnCrouch()
	}
	else
	{
		Crouch();
	}
}

void ABlasterCharacter::ReloadButtonPressed()
{
	if (Combat2 && Combat2->bHoldingTheFlag) return;
	if (bDisableGameplay) return;
	if (Combat2)
	{
		Combat2->Reload();
	}
}

void ABlasterCharacter::AimButtonPressed()
{
	if (Combat2 && Combat2->bHoldingTheFlag) return;
	if (bDisableGameplay) return;
	if (Combat2)
	{
		Combat2->SetAiming(true);
	}
}

void ABlasterCharacter::AimButtonReleased()
{
	if (Combat2 && Combat2->bHoldingTheFlag) return;
	if (bDisableGameplay) return;
	if (Combat2)
	{
		Combat2->SetAiming(false);
	}
}

float ABlasterCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}

/*
* エイムオフセット（視点移動）の処理
*/
void ABlasterCharacter::AimOffset(float DeltaTime)
{
	if (Combat2 && Combat2->EquippedWeapon == nullptr) return;
	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir) // standing still, not jumping
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.f || bIsInAir) // running, or jumping
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}
	
	CalculateAO_Pitch();
}

/*
* 操作キャラだけに関わるPitch部分の処理
* 操作キャラとそれ以外のキャラでエイムオフセットのレプリケーションを分けているのは、帯域幅を節約するため
*/
void ABlasterCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	/*
	if (HasAuthority && !IsLocallyControlled())
	{
		UE_LOG(LogTemp, Warning, TEXT("AO_Pitch: %f"), AO_Pitch);
	}*/
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// map pitch from [270, 360) to [-90, 0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);

		/* // Another way for map pitch from [270, 360) to [-90, 0)
		FRotator BaseAimRotation = GetBaseAimRotation();
		BaseAimRotation.Normalize();
		AO_Pitch = BaseAimRotation.Pitch;
		*/
	}
}

/*
* 操作キャラ以外のターンプレイスステイトを変更する処理
*/
void ABlasterCharacter::SimProxiesTurn()
{
	if (Combat2 == nullptr && Combat2->EquippedWeapon == nullptr) return;
	bRotateRootBone = false;
	float Speed = CalculateSpeed();
	if (Speed > 0)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

	//UE_LOG(LogTemp, Warning, TEXT("ProxyYaw: %f"), ProxyYaw);

	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

void ABlasterCharacter::Jump()
{
	if (Combat2 && Combat2->bHoldingTheFlag) return;
	if (bDisableGameplay) return;
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void ABlasterCharacter::FireButtonPressed()
{
	if (Combat2 && Combat2->bHoldingTheFlag) return;
	if (bDisableGameplay) return;
	if (Combat2)
	{
		Combat2->FireButtonPressed(true);
	}
}

void ABlasterCharacter::FireButtonReleased()
{
	if (Combat2 && Combat2->bHoldingTheFlag) return;
	if (bDisableGameplay) return;
	if (Combat2)
	{
		Combat2->FireButtonPressed(false);
	}
}

/*
* 操作キャラのターンプレイスステイトを変更する処理
*/
void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

/*
void ABlasterCharacter::MulticastHit_Implementation()
{
	PlayHitReactMontage();
}*/

/*
* キャラとカメラが近いとき、キャラを非表示に
*/
void ABlasterCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;
	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (Combat2 && Combat2->EquippedWeapon && Combat2->EquippedWeapon->GetWeaponMesh())
		{
			Combat2->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
		if (Combat2 && Combat2->SecondaryWeapon && Combat2->SecondaryWeapon->GetWeaponMesh())
		{
			Combat2->SecondaryWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat2 && Combat2->EquippedWeapon && Combat2->EquippedWeapon->GetWeaponMesh())
		{
			Combat2->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
		if (Combat2 && Combat2->SecondaryWeapon && Combat2->SecondaryWeapon->GetWeaponMesh())
		{
			Combat2->SecondaryWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void ABlasterCharacter::OnRep_Health(float LastHealth)
{
	UpdateHUDHealth();
	if (Health < LastHealth || !bElimmed) // This check need to avoid no working ElimAnimation on client by issue of animslot
	{
		PlayHitReactMontage();
	}
}

void ABlasterCharacter::OnRep_Shield(float LastShield)
{
	UpdateHUDShield();
	if (Shield < LastShield)
	{
		PlayHitReactMontage();
	}
}

void ABlasterCharacter::UpdateHUDHealth()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void ABlasterCharacter::UpdateHUDShield()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDShield(Shield, MaxShield);
	}
}

void ABlasterCharacter::UpdateHUDAmmo()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController && Combat2 && Combat2->EquippedWeapon)
	{
		BlasterPlayerController->SetHUDCarriedAmmo(Combat2->CarriedAmmo);
		BlasterPlayerController->SetHUDWeaponAmmo(Combat2->EquippedWeapon->GetAmmo());
	}
}

void ABlasterCharacter::SpawDefaultWeapon()
{
	BlasterGameMode = BlasterGameMode == nullptr ? GetWorld()->GetAuthGameMode<ABlasterGameMode>() : BlasterGameMode;
	UWorld* World = GetWorld();
	if (BlasterGameMode && World && !bElimmed && DefaultWeaponClass)
	{
		AWeapon* StartingWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		StartingWeapon->SetOwner(this);
		StartingWeapon->bDestroyWeapon = true;
		if (Combat2)
		{
			Combat2->EquipWeapon(StartingWeapon);
		}
	}
}

/*
* 一度だけ呼ばれる初期化処理
* ゲームに途中参加したプレイヤー用
*/
void ABlasterCharacter::PollInit()
{
	if (BlasterPlayerState == nullptr)
	{
		// Only for initial score (Display show "Score: 0")
		BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
		if (BlasterPlayerState)
		{
			OnPlayerStateInitialized();

			ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));

			if (BlasterGameState && BlasterGameState->TopScoringPlayers.Contains(BlasterPlayerState))
			{
				MulticastGainedTheLead();
			}
			//BlasterPlayerState = BlasterPlayerState == nullptr ? GetPlayerState<ABlasterPlayerState>() : BlasterPlayerState;
			//if (BlasterPlayerState)
			//{
			//	SetTeamColor(BlasterPlayerState->Team);
			//}
		}
	}
}

void ABlasterCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}

void ABlasterCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ABlasterCharacter::UpdateDissolveMaterial);
	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (IsLocallyControlled()) // Serverのコントローラーのみ起動できるようになる
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(false);
		}
	}

	OverlappingWeapon = Weapon; // OnRep_OverlappingWeaponがここで呼び出される
	
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(TEXT("Connect string: %s"), TEXT("aaa")));
}

bool ABlasterCharacter::IsWeaponEquipped() // Called BlasterAnimInstance.cpp
{
	return (Combat2 && Combat2->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming()
{
	return (Combat2 && Combat2->bAiming);
}

AWeapon* ABlasterCharacter::GetEquippedWeapon()
{
	if (Combat2 == nullptr) return nullptr;
	return Combat2->EquippedWeapon;
}

FVector ABlasterCharacter::GetHitTarget() const
{
	if (Combat2 == nullptr) return FVector();
	return Combat2->HitTarget;
}

ECombatState ABlasterCharacter::GetCombatState() const
{
	if (Combat2 == nullptr) return ECombatState::ECS_MAX;
	return Combat2->CombatState;
}

bool ABlasterCharacter::IsLocallyReloading()
{
	if (Combat2 == nullptr) return false;
	return Combat2->bLocallyReloading;
}

bool ABlasterCharacter::IsHoldingTheFlag()
{
	if (Combat2 == nullptr) return false;
	return Combat2->bHoldingTheFlag;
}

ETeam ABlasterCharacter::GetTeam()
{
	BlasterPlayerState = BlasterPlayerState == nullptr ? GetPlayerState<ABlasterPlayerState>() : BlasterPlayerState;
	if (BlasterPlayerState == nullptr) return ETeam::ET_NoTeam;
	return BlasterPlayerState->GetTeam();
}

void ABlasterCharacter::SetHoldingTheFlag(bool bHolding)
{
	if (Combat2 == nullptr) return;
	Combat2->bHoldingTheFlag = bHolding;
}