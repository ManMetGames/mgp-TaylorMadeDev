// Fill out your copyright notice in the Description page of Project Settings.

#include "HybridSpriteCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "UObject/ConstructorHelpers.h"

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
	}
}

void AHybridSpriteCharacter::Move(const FInputActionValue& Value)
{
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
}

void AHybridSpriteCharacter::UpdateAnimation()
{
	if (!Sprite)
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

	if (bIsMoving)
	{
		const float Speed = HorizontalVelocity.Size();
		const FRotator YawRotation(0.0f, CameraYaw, 0.0f);
		const FVector ForwardDirection = -FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		float LocalForward = 0.0f;
		float LocalRight = 0.0f;
		if (Speed > KINDA_SMALL_NUMBER)
		{
			LocalForward = FVector::DotProduct(HorizontalVelocity, ForwardDirection) / Speed;
			LocalRight = FVector::DotProduct(HorizontalVelocity, RightDirection) / Speed;
		}

		if (FMath::Abs(LocalForward) < AnimationInputDeadzone && FMath::Abs(LastMoveInputY) > AnimationInputDeadzone)
		{
			LocalForward = FMath::Sign(LastMoveInputY);
		}
		if (FMath::Abs(LocalRight) < AnimationInputDeadzone && FMath::Abs(LastMoveInputX) > AnimationInputDeadzone)
		{
			LocalRight = FMath::Sign(LastMoveInputX);
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