// Fill out your copyright notice in the Description page of Project Settings.

#include "HybridSpriteCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "Components/WidgetComponent.h"
#include "HealthBarWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AHybridSpriteCharacter::AHybridSpriteCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	GetCapsuleComponent()->InitCapsuleSize(20.0f, 44.0f);

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;

	VisualRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VisualRoot"));
	VisualRoot->SetupAttachment(GetRootComponent());

	Sprite = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Sprite"));
	Sprite->SetupAttachment(VisualRoot);
	Sprite->bEditableWhenInherited = true;
	Sprite->SetRelativeLocation(FVector(0.0f, 0.0f, -44.0f));
	Sprite->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	Sprite->SetUsingAbsoluteRotation(false);
	Sprite->SetVisibility(true);
	Sprite->SetHiddenInGame(false);
	Sprite->SetLooping(true);
	Sprite->SetPlayRate(1.0f);

	WeaponSlashSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("WeaponSlashSprite"));
	WeaponSlashSprite->SetupAttachment(VisualRoot);
	WeaponSlashSprite->SetRelativeLocation(FVector(SlashForwardOffset, 0.0f, 0.0f));
	WeaponSlashSprite->SetRelativeRotation(FRotator(0.0f, -55.0f, 0.0f));
	WeaponSlashSprite->SetHiddenInGame(true);
	WeaponSlashSprite->SetVisibility(false);
	WeaponSlashSprite->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponSlashHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponSlashHitbox"));
	WeaponSlashHitbox->SetupAttachment(WeaponSlashSprite);
	WeaponSlashHitbox->SetBoxExtent(FVector(18.0f, SlashSweepWidth * 0.5f, 22.0f));
	WeaponSlashHitbox->SetRelativeLocation(FVector(20.0f, 0.0f, 0.0f));
	WeaponSlashHitbox->SetHiddenInGame(true);
	WeaponSlashHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponSlashHitbox->SetCollisionObjectType(ECC_WorldDynamic);
	WeaponSlashHitbox->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponSlashHitbox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(RootComponent);
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	HealthBarWidget->SetDrawSize(FVector2D(100.0f, 20.0f));

	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> IdleDownAsset(TEXT("/Game/PaperAssets/FlipBooks/Move/Idle_Down.Idle_Down"));
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> IdleSideAsset(TEXT("/Game/PaperAssets/FlipBooks/Move/Idle_Side.Idle_Side"));
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> IdleUpAsset(TEXT("/Game/PaperAssets/FlipBooks/Move/Idle_Up.Idle_Up"));
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> WalkDownAsset(TEXT("/Game/PaperAssets/FlipBooks/Move/Walk_Down.Walk_Down"));
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> WalkSideAsset(TEXT("/Game/PaperAssets/FlipBooks/Move/Walk_Side.Walk_Side"));
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> WalkUpAsset(TEXT("/Game/PaperAssets/FlipBooks/Move/Walk_Up.Walk_Up"));
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> IdleUpSideAsset(TEXT("/Game/PaperAssets/FlipBooks/Move/Idle_Up_Side.Idle_Up_Side"));
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> IdleDownSideAsset(TEXT("/Game/PaperAssets/FlipBooks/Move/Idle_Down_Side.Idle_Down_Side"));
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> WalkUpSideAsset(TEXT("/Game/PaperAssets/FlipBooks/Move/Walk_Up_Side.Walk_Up_Side"));
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> WalkDownSideAsset(TEXT("/Game/PaperAssets/FlipBooks/Move/Walk_Down_Side.Walk_Down_Side"));

	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> IdleDownAssetLegacy(TEXT("/Game/PaperAssets/FlipBooks/Idle_Down.Idle_Down"));
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> IdleSideAssetLegacy(TEXT("/Game/PaperAssets/FlipBooks/Idle_Side.Idle_Side"));
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> IdleUpAssetLegacy(TEXT("/Game/PaperAssets/FlipBooks/Idle_Up.Idle_Up"));
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> WalkDownAssetLegacy(TEXT("/Game/PaperAssets/FlipBooks/Walk_Down.Walk_Down"));
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> WalkSideAssetLegacy(TEXT("/Game/PaperAssets/FlipBooks/Walk_Side.Walk_Side"));
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> WalkUpAssetLegacy(TEXT("/Game/PaperAssets/FlipBooks/Walk_Up.Walk_Up"));

	IdleDownFlipbook = IdleDownAsset.Succeeded() ? IdleDownAsset.Object : (IdleDownAssetLegacy.Succeeded() ? IdleDownAssetLegacy.Object : nullptr);
	IdleSideFlipbook = IdleSideAsset.Succeeded() ? IdleSideAsset.Object : (IdleSideAssetLegacy.Succeeded() ? IdleSideAssetLegacy.Object : nullptr);
	IdleUpFlipbook = IdleUpAsset.Succeeded() ? IdleUpAsset.Object : (IdleUpAssetLegacy.Succeeded() ? IdleUpAssetLegacy.Object : nullptr);
	WalkDownFlipbook = WalkDownAsset.Succeeded() ? WalkDownAsset.Object : (WalkDownAssetLegacy.Succeeded() ? WalkDownAssetLegacy.Object : nullptr);
	WalkSideFlipbook = WalkSideAsset.Succeeded() ? WalkSideAsset.Object : (WalkSideAssetLegacy.Succeeded() ? WalkSideAssetLegacy.Object : nullptr);
	WalkUpFlipbook = WalkUpAsset.Succeeded() ? WalkUpAsset.Object : (WalkUpAssetLegacy.Succeeded() ? WalkUpAssetLegacy.Object : nullptr);
	IdleUpSideFlipbook = IdleUpSideAsset.Succeeded() ? IdleUpSideAsset.Object : nullptr;
	IdleDownSideFlipbook = IdleDownSideAsset.Succeeded() ? IdleDownSideAsset.Object : nullptr;
	WalkUpSideFlipbook = WalkUpSideAsset.Succeeded() ? WalkUpSideAsset.Object : nullptr;
	WalkDownSideFlipbook = WalkDownSideAsset.Succeeded() ? WalkDownSideAsset.Object : nullptr;

	if (IdleDownFlipbook)
	{
		Sprite->SetFlipbook(IdleDownFlipbook);
		Sprite->PlayFromStart();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HybridSpriteCharacter: Could not load Idle_Down flipbook."));
	}

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = TargetArmLength;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 10.0f;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraRotationLagSpeed = 12.0f;
	SpringArm->bDoCollisionTest = true;
	SpringArm->SocketOffset = FVector(0.0f, 25.0f, 40.0f);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 55.0f;
}

void AHybridSpriteCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (WeaponSlashHitbox)
	{
		WeaponSlashHitbox->OnComponentBeginOverlap.AddDynamic(this, &AHybridSpriteCharacter::OnAttackHitboxOverlap);
	}

	if (SpringArm)
	{
		InitialSocketOffset = SpringArm->SocketOffset;
	}

	if (!Sprite || !Sprite->GetFlipbook())
	{
		UE_LOG(LogTemp, Warning, TEXT("HybridSpriteCharacter: Sprite component has no flipbook assigned at BeginPlay."));
	}

	SpringArm->SetRelativeRotation(FRotator(CameraPitch, CameraYaw, 0.0f));
	SpringArm->TargetArmLength = TargetArmLength;

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
				LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (DefaultMappingContext)
				{
					Subsystem->AddMappingContext(DefaultMappingContext, 0);
				}
			}
		}
	}
}

void AHybridSpriteCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCamera(DeltaTime);
	UpdateAttackSwing(DeltaTime);
	UpdateAnimation();
}

void AHybridSpriteCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHybridSpriteCharacter::Move);
		}
	if (HealthBarWidget)
	{
		// Ensure the widget instance exists before trying to access it.
		HealthBarWidget->InitWidget();

		if (UHealthBarWidget* HealthWidget = Cast<UHealthBarWidget>(HealthBarWidget->GetUserWidgetObject()))
		{
			HealthWidget->SetSourceCharacter(this);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("HybridSpriteCharacter: HealthBarWidget exists but could not be cast to UHealthBarWidget at BeginPlay."));
		}
	}

		if (LookXAction)
		{
			EnhancedInput->BindAction(LookXAction, ETriggerEvent::Triggered, this, &AHybridSpriteCharacter::LookX);
		}

		if (LookYAction)
		{
			EnhancedInput->BindAction(LookYAction, ETriggerEvent::Triggered, this, &AHybridSpriteCharacter::LookY);
		}

		if (ZoomAction)
		{
			EnhancedInput->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &AHybridSpriteCharacter::Zoom);
		}

		if (AttackAction)
		{
			EnhancedInput->BindAction(AttackAction, ETriggerEvent::Started, this, &AHybridSpriteCharacter::StartAttack);
		}

		if (BlockAction)
		{
			EnhancedInput->BindAction(BlockAction, ETriggerEvent::Started, this, &AHybridSpriteCharacter::StartBlock);
			EnhancedInput->BindAction(BlockAction, ETriggerEvent::Completed, this, &AHybridSpriteCharacter::EndBlock);
		}

		if (LockOnAction)
		{
			EnhancedInput->BindAction(LockOnAction, ETriggerEvent::Started, this, &AHybridSpriteCharacter::ToggleTargetLock);
		}
	}
}

void AHybridSpriteCharacter::Move(const FInputActionValue& Value)
{
	if (!CanMove())
	{
		return;
	}

	const FVector2D MoveInput = Value.Get<FVector2D>();
	LastMoveInputX = MoveInput.X;
	LastMoveInputY = MoveInput.Y;

	if (Controller == nullptr)
	{
		return;
	}

	const FRotator YawRotation(0.0f, CameraYaw, 0.0f);

	const FVector ForwardDirection = -FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MoveInput.Y);
	AddMovementInput(RightDirection, MoveInput.X);
}

void AHybridSpriteCharacter::LookX(const FInputActionValue& Value)
{
	const float AxisValue = Value.Get<float>();
	CameraYaw += AxisValue * LookSensitivityX;
}

void AHybridSpriteCharacter::LookY(const FInputActionValue& Value)
{
	const float AxisValue = Value.Get<float>();

	CameraPitch = FMath::Clamp(
		CameraPitch + AxisValue * LookSensitivityY,
		MinCameraPitch,
		MaxCameraPitch
	);
}

void AHybridSpriteCharacter::Zoom(const FInputActionValue& Value)
{
	const float AxisValue = Value.Get<float>();

	TargetArmLength = FMath::Clamp(
		TargetArmLength + (-AxisValue * ZoomSpeed),
		MinZoom,
		MaxZoom
	);
}

void AHybridSpriteCharacter::UpdateCamera(float DeltaTime)
{
	if (!SpringArm)
	{
		return;
	}

	// Target Lock overrides the normal camera yaw smoothly
	if (IsValid(LockedTarget))
	{
		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LockedTarget->GetActorLocation());
		CameraYaw = FMath::FInterpTo(CameraYaw, LookAtRot.Yaw, DeltaTime, 10.0f);
	}

	const FRotator TargetRotation(CameraPitch, CameraYaw, 0.0f);
	SpringArm->SetRelativeRotation(TargetRotation);

	if (bRotateSpriteWithCamera && VisualRoot)
	{
		VisualRoot->SetWorldRotation(FRotator(0.0f, CameraYaw, 0.0f));
	}

	SpringArm->TargetArmLength = FMath::FInterpTo(
		SpringArm->TargetArmLength,
		TargetArmLength,
		DeltaTime,
		ZoomInterpSpeed
	);

	if (bEnableCameraBob && GetCharacterMovement())
	{
		FVector Velocity = GetVelocity();
		Velocity.Z = 0.0f;
		float Speed = Velocity.Size();
		float MaxSpeed = GetCharacterMovement()->MaxWalkSpeed > 0.0f ? GetCharacterMovement()->MaxWalkSpeed : 400.0f;

		if (Speed > KINDA_SMALL_NUMBER && GetCharacterMovement()->IsMovingOnGround())
		{
			CameraBobTime += DeltaTime * CameraBobSpeed * (Speed / MaxSpeed);
			float BobOffset = FMath::Sin(CameraBobTime) * CameraBobAmount;
			SpringArm->SocketOffset = InitialSocketOffset + FVector(0.0f, 0.0f, BobOffset);
		}
		else
		{
			CameraBobTime = 0.0f;
			SpringArm->SocketOffset = FMath::VInterpTo(
				SpringArm->SocketOffset, 
				InitialSocketOffset, 
				DeltaTime, 
				15.0f
			);
		}
	}
}

void AHybridSpriteCharacter::UpdateAttackSwing(float DeltaTime)
{
	// If we are not actively swinging, hide the sprite and do nothing
	if (!bAttackSwingActive || !WeaponSlashSprite)
	{
		if (WeaponSlashSprite && !WeaponSlashSprite->bHiddenInGame)
		{
			WeaponSlashSprite->SetHiddenInGame(true);
			WeaponSlashSprite->SetVisibility(false, true);
		}
		return;
	}

	// Turn the sprite on
	WeaponSlashSprite->SetHiddenInGame(false);
	WeaponSlashSprite->SetVisibility(true, true);

	AttackSwingElapsed += DeltaTime;
	
	// Create a sweeping motion from left to right (-55 degrees to +55 degrees)
	float SwingProgress = FMath::Clamp(AttackSwingElapsed / AttackActiveTime, 0.0f, 1.0f);
	
	// Make it arc around the player
	float StartAngle = -55.0f;
	float EndAngle = 55.0f;
	float CurrentAngle = FMath::Lerp(StartAngle, EndAngle, SwingProgress);

	FRotator SwingRotation(0.0f, CurrentAngle, 0.0f);
	WeaponSlashSprite->SetRelativeRotation(SwingRotation);

	FVector ForwardOffset(SlashForwardOffset, 0.0f, 0.0f);
	WeaponSlashSprite->SetRelativeLocation(ForwardOffset);
}

void AHybridSpriteCharacter::UpdateAnimation()
{
	if (!Sprite)
	{
		return;
	}

	if (!CanMove())
	{
		return;
	}

	const FVector Velocity = GetVelocity();
	const FVector HorizontalVelocity(Velocity.X, Velocity.Y, 0.0f);
	const bool bIsMoving = HorizontalVelocity.SizeSquared() > FMath::Square(MovementAnimationThreshold);
	const float HorizontalSpeed = HorizontalVelocity.Size();
	const float MaxSpeed = FMath::Max(GetCharacterMovement() ? GetCharacterMovement()->MaxWalkSpeed : 1.0f, 1.0f);
	const float SpeedAlpha = FMath::Clamp(HorizontalSpeed / MaxSpeed, 0.0f, 1.0f);
	const float TargetPlayRate = bIsMoving
		? FMath::Lerp(MinWalkAnimationPlayRate, MaxWalkAnimationPlayRate, SpeedAlpha)
		: IdleAnimationPlayRate;
	Sprite->SetPlayRate(TargetPlayRate);

	EHybridSpriteDirection Direction = LastFacingDirection;
	float SideFacingSign = LastSideFacingSign;
	int8 SideVerticalVariant = LastSideVerticalVariant;

	bool bTargetLocked = IsValid(LockedTarget);

	if (bIsMoving || bTargetLocked)
	{
		const float Speed = HorizontalVelocity.Size();
		const FRotator YawRotation(0.0f, CameraYaw, 0.0f);
		const FVector ForwardDirection = -FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		float LocalForward = 0.0f;
		float LocalRight = 0.0f;

		if (bTargetLocked)
		{
			FVector ToTarget = (LockedTarget->GetActorLocation() - GetActorLocation());
			ToTarget.Z = 0.0f;
			ToTarget.Normalize();
			
			LocalForward = FVector::DotProduct(ToTarget, ForwardDirection);
			LocalRight = FVector::DotProduct(ToTarget, RightDirection);
		}
		else if (Speed > KINDA_SMALL_NUMBER)
		{
			LocalForward = FVector::DotProduct(HorizontalVelocity, ForwardDirection) / Speed;
			LocalRight = FVector::DotProduct(HorizontalVelocity, RightDirection) / Speed;
			
			if (FMath::Abs(LocalForward) < AnimationInputDeadzone && FMath::Abs(LastMoveInputY) > AnimationInputDeadzone)
			{
				LocalForward = FMath::Sign(LastMoveInputY);
			}
			if (FMath::Abs(LocalRight) < AnimationInputDeadzone && FMath::Abs(LastMoveInputX) > AnimationInputDeadzone)
			{
				LocalRight = FMath::Sign(LastMoveInputX);
			}
		}

		const bool bHasForward = FMath::Abs(LocalForward) > AnimationInputDeadzone;
		const bool bHasRight = FMath::Abs(LocalRight) > AnimationInputDeadzone;

		if (bHasRight)
		{
			Direction = EHybridSpriteDirection::Side;
			SideFacingSign = (LocalRight < 0.0f) ? -1.0f : 1.0f;

			if (bHasForward)
			{
				SideVerticalVariant = (LocalForward > 0.0f) ? -1 : 1;
			}
			else
			{
				SideVerticalVariant = 0;
			}
		}
		else if (bHasForward)
		{
			Direction = (LocalForward > 0.0f) ? EHybridSpriteDirection::Down : EHybridSpriteDirection::Up;
			SideVerticalVariant = 0;
		}

		LastFacingDirection = Direction;
		LastSideFacingSign = SideFacingSign;
		LastSideVerticalVariant = SideVerticalVariant;
	}

	if (Direction == EHybridSpriteDirection::Side)
	{
		FVector NewScale = Sprite->GetRelativeScale3D();
		const float AbsScaleX = FMath::Abs(NewScale.X) > KINDA_SMALL_NUMBER ? FMath::Abs(NewScale.X) : 1.0f;
		NewScale.X = (SideFacingSign < 0.0f) ? -AbsScaleX : AbsScaleX;
		Sprite->SetRelativeScale3D(NewScale);
	}
	else
	{
		FVector NewScale = Sprite->GetRelativeScale3D();
		NewScale.X = FMath::Abs(NewScale.X) > KINDA_SMALL_NUMBER ? FMath::Abs(NewScale.X) : 1.0f;
		Sprite->SetRelativeScale3D(NewScale);
	}

	SetFlipbookForState(bIsMoving, Direction);
}

void AHybridSpriteCharacter::SetFlipbookForState(bool bIsMoving, EHybridSpriteDirection Direction)
{
	UPaperFlipbook* Desired = nullptr;

	if (bIsMoving)
	{
		switch (Direction)
		{
		case EHybridSpriteDirection::Up:
			Desired = WalkUpFlipbook;
			break;
		case EHybridSpriteDirection::Side:
			if (LastSideVerticalVariant > 0)
			{
				Desired = WalkUpSideFlipbook ? WalkUpSideFlipbook : WalkSideFlipbook;
			}
			else if (LastSideVerticalVariant < 0)
			{
				Desired = WalkDownSideFlipbook ? WalkDownSideFlipbook : WalkSideFlipbook;
			}
			else
			{
				Desired = WalkSideFlipbook;
			}
			break;
		default:
			Desired = WalkDownFlipbook;
			break;
		}
	}
	else
	{
		switch (Direction)
		{
		case EHybridSpriteDirection::Up:
			Desired = IdleUpFlipbook;
			break;
		case EHybridSpriteDirection::Side:
			if (LastSideVerticalVariant > 0)
			{
				Desired = IdleUpSideFlipbook ? IdleUpSideFlipbook : IdleSideFlipbook;
			}
			else if (LastSideVerticalVariant < 0)
			{
				Desired = IdleDownSideFlipbook ? IdleDownSideFlipbook : IdleSideFlipbook;
			}
			else
			{
				Desired = IdleSideFlipbook;
			}
			break;
		default:
			Desired = IdleDownFlipbook;
			break;
		}
	}

	if (!Desired || !Sprite)
	{
		return;
	}

	if (Sprite->GetFlipbook() != Desired)
	{
		Sprite->SetFlipbook(Desired);
		Sprite->SetLooping(true);
		Sprite->PlayFromStart();
	}
	else if (!Sprite->IsPlaying())
	{
		Sprite->Play();
	}
}

// -------------------------------------------------------------------------
// Combat State Machine & Attack Mechanics
// -------------------------------------------------------------------------

void AHybridSpriteCharacter::SetCombatState(ECombatState NewState)
{
	CombatState = NewState;
}

bool AHybridSpriteCharacter::IsInState(ECombatState StateToCheck) const
{
	return CombatState == StateToCheck;
}

bool AHybridSpriteCharacter::CanAttack() const
{
	return (CombatState == ECombatState::Idle || CombatState == ECombatState::Moving);
}

bool AHybridSpriteCharacter::CanMove() const
{
	return CombatState == ECombatState::Idle || CombatState == ECombatState::Moving;
}

void AHybridSpriteCharacter::SetWeaponEquipped(bool bNewWeaponEquipped)
{
	bWeaponEquipped = bNewWeaponEquipped;
}

void AHybridSpriteCharacter::StartAttack()
{
	if (!CanAttack() || !IsWeaponEquipped())
	{
		return;
	}

	SetCombatState(ECombatState::Attacking);
	bAttackSwingActive = true;
	AttackSwingElapsed = 0.0f;

	if (WeaponSlashSprite)
	{
		WeaponSlashSprite->SetSprite(WeaponSlashSpriteAsset);
		WeaponSlashSprite->SetHiddenInGame(false);
		WeaponSlashSprite->SetVisibility(true, true);
		WeaponSlashSprite->SetRelativeLocation(FVector(SlashForwardOffset, 0.0f, 0.0f));
		WeaponSlashSprite->SetRelativeRotation(FRotator(0.0f, -55.0f, 0.0f));
	}
	
	// Print to screen to prove we are attacking
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow, TEXT("Attack Windup..."));
	}

	// Step 1: Wait for windup to finish, then activate hitbox
	GetWorldTimerManager().SetTimer(
		AttackWindupTimerHandle,
		this,
		&AHybridSpriteCharacter::BeginAttackActiveFrames,
		AttackWindupTime,
		false
	);
}

void AHybridSpriteCharacter::BeginAttackActiveFrames()
{
	if (!IsInState(ECombatState::Attacking)) return;

	if (WeaponSlashHitbox)
	{
		WeaponSlashHitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red, TEXT("Attack ACTIVE! Hitbox is lethal."));
	}

	// Make the trace or hitbox active here (will do this next time)

	// Step 2: Wait for active frames to end
	GetWorldTimerManager().SetTimer(
		AttackActiveTimerHandle,
		this,
		&AHybridSpriteCharacter::EndAttackActiveFrames,
		AttackActiveTime,
		false
	);
}

void AHybridSpriteCharacter::EndAttackActiveFrames()
{
	if (!IsInState(ECombatState::Attacking)) return;

	if (WeaponSlashHitbox)
	{
		WeaponSlashHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Orange, TEXT("Attack Recovery..."));
	}

	// Disable the trace or hitbox here

	// Step 3: Wait for recovery to end
	GetWorldTimerManager().SetTimer(
		AttackRecoveryTimerHandle,
		this,
		&AHybridSpriteCharacter::FinishAttack,
		AttackRecoveryTime,
		false
	);
}

void AHybridSpriteCharacter::FinishAttack()
{
	if (!IsInState(ECombatState::Attacking)) return;

	if (WeaponSlashSprite)
	{
		WeaponSlashSprite->SetHiddenInGame(true);
		WeaponSlashSprite->SetVisibility(false, true);
		WeaponSlashSprite->SetRelativeLocation(FVector(SlashForwardOffset, 0.0f, 0.0f));
		WeaponSlashSprite->SetRelativeRotation(FRotator(0.0f, -55.0f, 0.0f));
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, TEXT("Attack finished, back to Idle."));
	}

	// Reset state based on whether we are currently moving or not
	if (GetVelocity().SizeSquared() > 0)
	{
		SetCombatState(ECombatState::Moving);
	}
	else
	{
		SetCombatState(ECombatState::Idle);
	}
}

void AHybridSpriteCharacter::ToggleTargetLock()
{
	if (GEngine) 
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("ToggleTargetLock Fired!"));
	}

	if (IsValid(LockedTarget))
	{
		LockedTarget = nullptr;
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, TEXT("Target Dropped"));
		return;
	}

	TArray<AActor*> OverlappedActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	UKismetSystemLibrary::SphereOverlapActors(
		this,
		GetActorLocation(),
		TargetLockRadius,
		ObjectTypes,
		AActor::StaticClass(),
		TArray<AActor*>{this},
		OverlappedActors
	);

	AActor* BestTarget = nullptr;
	float ClosestDistSq = MAX_flt;

	for (AActor* Actor : OverlappedActors)
	{
		if (Actor && Actor->ActorHasTag(FName("Enemy")))
		{
			float DistSq = FVector::DistSquared(GetActorLocation(), Actor->GetActorLocation());
			if (DistSq < ClosestDistSq)
			{
				ClosestDistSq = DistSq;
				BestTarget = Actor;
			}
		}
	}

	if (BestTarget)
	{
		LockedTarget = BestTarget;
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Locked: %s"), *BestTarget->GetName()));
	}
	else
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("No enemy in range"));
	}
}

void AHybridSpriteCharacter::OnAttackHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this || OtherActor == nullptr) return;

	// Only hit things tagged Enemy
	if (OtherActor->ActorHasTag(FName("Enemy")))
	{
		// Apply Damage
		UGameplayStatics::ApplyDamage(OtherActor, AttackDamage, GetController(), this, UDamageType::StaticClass());
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Purple, FString::Printf(TEXT("Dealt %f Damage to %s!"), AttackDamage, *OtherActor->GetName()));
		}
	}
}

float AHybridSpriteCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float Incoming = DamageAmount;

	bool bParried = false;

	if (bIsBlocking && DamageCauser)
	{
		// Check if attacker is in front
		FVector ToAttacker = (DamageCauser->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		FVector Forward = GetActorForwardVector();
		float Dot = FVector::DotProduct(Forward, ToAttacker);

		if (Dot > 0.0f)
		{
			if (bCanParry)
			{
				// Parry: negate damage and stun attacker
				Incoming = 0.0f;
				bParried = true;

				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 1.8f, FColor::Yellow, TEXT("Parried!"));
				}

				if (AHybridSpriteCharacter* AttackerChar = Cast<AHybridSpriteCharacter>(DamageCauser))
				{
					AttackerChar->SetCombatState(ECombatState::Stunned);
					AttackerChar->GetWorldTimerManager().SetTimer(AttackerChar->StunTimerHandle, AttackerChar, &AHybridSpriteCharacter::RecoverFromStun, ParryStunDuration, false);
				}
			}
			else
			{
				// Regular block: reduce damage
				Incoming = DamageAmount * BlockDamageMultiplier;
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Cyan, FString::Printf(TEXT("Blocked %f -> %f"), DamageAmount, Incoming));
				}
			}
		}
	}

	float ActualDamage = Super::TakeDamage(Incoming, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage > 0.0f)
	{
		CurrentHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.0f, MaxHealth);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("%s took %f damage! Health: %f / %f"), *GetName(), ActualDamage, CurrentHealth, MaxHealth));
		}
	}

	return ActualDamage;
}

// -------------------------------------------------------------------------
// Blocking / Parry
// -------------------------------------------------------------------------

void AHybridSpriteCharacter::StartBlock()
{
	if (!CanMove()) return;

	bIsBlocking = true;
	bCanParry = true;
	SetCombatState(ECombatState::Blocking);

	// After parry window, parry is no longer possible
	GetWorldTimerManager().SetTimer(ParryTimerHandle, this, &AHybridSpriteCharacter::ClearParry, ParryWindow, false);

	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 0.8f, FColor::Blue, TEXT("Block start"));
}

void AHybridSpriteCharacter::EndBlock()
{
	bIsBlocking = false;
	bCanParry = false;
	SetCombatState(ECombatState::Idle);
	GetWorldTimerManager().ClearTimer(ParryTimerHandle);

	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 0.8f, FColor::Blue, TEXT("Block end"));
}

void AHybridSpriteCharacter::ClearParry()
{
	bCanParry = false;
}

void AHybridSpriteCharacter::RecoverFromStun()
{
	// Recover into Idle state
	SetCombatState(ECombatState::Idle);
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Orange, TEXT("Recovered from stun"));
}
