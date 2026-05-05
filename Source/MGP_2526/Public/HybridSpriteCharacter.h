// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "HybridSpriteCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UPaperFlipbookComponent;
class UPaperSpriteComponent;
class UBoxComponent;
class USceneComponent;
class UInputMappingContext;
class UInputAction;
class UPaperFlipbook;
class UPaperSprite;
class UHealthBarWidget;
class USoundBase;

UENUM()
enum class EHybridSpriteDirection : uint8
{
	Down,
	Side,
	Up
};

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	Idle		UMETA(DisplayName = "Idle"),
	Moving		UMETA(DisplayName = "Moving"),
	Attacking	UMETA(DisplayName = "Attacking"),
	Blocking	UMETA(DisplayName = "Blocking"),
	Parrying	UMETA(DisplayName = "Parrying"),
	HitReact	UMETA(DisplayName = "Hit React"),
	Stunned		UMETA(DisplayName = "Stunned"),
	Dead		UMETA(DisplayName = "Dead")
};

UCLASS()
class MGP_2526_API AHybridSpriteCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AHybridSpriteCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|State")
	ECombatState CombatState = ECombatState::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Visual")
	UPaperSpriteComponent* WeaponSlashSprite;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Visual")
	UBoxComponent* WeaponSlashHitbox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class UWidgetComponent* HealthBarWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float CurrentHealth = 100.0f;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Stats")
	float GetHealthPercent() const { return MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Camera")
	float GetCameraYaw() const { return CameraYaw; }

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Visual")
	UPaperSprite* WeaponSlashSpriteAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Attack")
	float AttackDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Attack")
	float AttackWindupTime = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Attack")
	float AttackActiveTime = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Attack")
	float AttackRecoveryTime = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Attack")
	float SlashTravelDistance = 95.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Attack")
	float SlashSweepWidth = 70.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Attack")
	float SlashForwardOffset = 55.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Audio")
	USoundBase* ParrySoundAsset = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Audio")
	USoundBase* BlockSoundAsset = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Audio")
	USoundBase* SwingSoundAsset = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Audio")
	USoundBase* HitSoundAsset = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|State")
	bool bWeaponEquipped = true;

	UFUNCTION(BlueprintCallable, Category = "Combat|State")
	void SetCombatState(ECombatState NewState);

	UFUNCTION(BlueprintCallable, Category = "Combat|State")
	bool IsInState(ECombatState StateToCheck) const;

	UFUNCTION(BlueprintCallable, Category = "Combat|State")
	bool CanMove() const;

	UFUNCTION(BlueprintCallable, Category = "Combat|State")
	bool CanAttack() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Combat|State")
	bool IsWeaponEquipped() const;

	UFUNCTION(BlueprintCallable, Category = "Combat|State")
	void SetWeaponEquipped(bool bNewWeaponEquipped);

	UFUNCTION(BlueprintCallable, Category = "Combat|Attack")
	void StartAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat|Attack")
	void BeginAttackActiveFrames();

	UFUNCTION(BlueprintCallable, Category = "Combat|Attack")
	void EndAttackActiveFrames();

	UFUNCTION(BlueprintCallable, Category = "Combat|Attack")
	void FinishAttack();

	UFUNCTION()
	void OnAttackHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* VisualRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPaperFlipbookComponent* Sprite;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* LookXAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* LookYAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* ZoomAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* AttackAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* LockOnAction;

	// Input for blocking/parry
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* BlockAction;

	FTimerHandle AttackWindupTimerHandle;
	FTimerHandle AttackActiveTimerHandle;
	FTimerHandle AttackRecoveryTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UPaperFlipbook* IdleDownFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UPaperFlipbook* IdleSideFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UPaperFlipbook* IdleUpSideFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UPaperFlipbook* IdleDownSideFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UPaperFlipbook* IdleUpFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UPaperFlipbook* WalkDownFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UPaperFlipbook* WalkSideFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UPaperFlipbook* WalkUpSideFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UPaperFlipbook* WalkDownSideFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UPaperFlipbook* WalkUpFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float MovementAnimationThreshold = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float AnimationInputDeadzone = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float IdleAnimationPlayRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float MinWalkAnimationPlayRate = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float MaxWalkAnimationPlayRate = 1.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bRotateSpriteWithCamera = true;

	float LastMoveInputX = 0.0f;
	float LastMoveInputY = 0.0f;
	EHybridSpriteDirection LastFacingDirection = EHybridSpriteDirection::Down;
	float LastSideFacingSign = 1.0f;
	int8 LastSideVerticalVariant = 0; // -1 DownSide, 0 Side, +1 UpSide

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraYaw = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraPitch = -10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MinCameraPitch = -25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MaxCameraPitch = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float LookSensitivityX = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float LookSensitivityY = -1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float TargetArmLength = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MinZoom = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MaxZoom = 850.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float ZoomSpeed = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float ZoomInterpSpeed = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Bob")
	bool bEnableCameraBob = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Bob")
	float CameraBobSpeed = 22.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Bob")
	float CameraBobAmount = 1.5f;

	float CameraBobTime = 0.0f;
	FVector InitialSocketOffset;
	float AttackSwingElapsed = 0.0f;
	bool bAttackSwingActive = false;

	// Targeting System
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Targeting")
	AActor* LockedTarget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Targeting")
	float TargetLockRadius = 1500.0f;

	UFUNCTION(BlueprintCallable, Category = "Combat|Targeting")
	void ToggleTargetLock();

	UFUNCTION()
	void ClearLockOnDoubleTap();

	// Blocking API
	UFUNCTION(BlueprintCallable, Category = "Combat|Defense")
	void StartBlock();

	UFUNCTION(BlueprintCallable, Category = "Combat|Defense")
	void EndBlock();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Defense")
	bool bIsBlocking = false;

	// Short window after starting block that counts as a parry (completely negates and staggers)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Defense")
	float ParryWindow = 0.18f;

	// Debug sphere shown while parry is active
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Defense")
	bool bShowParryDebugSphere = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Defense")
	float ParryDebugSphereRadius = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Defense")
	float ParryDebugSphereForwardOffset = 45.0f;

	// How long after ending a block before you can block again
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Defense")
	float BlockCooldown = 1.0f;

	// Double-tap timing for switching enemy lock-on targets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Targeting")
	float LockOnDoubleTapWindow = 0.3f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Targeting")
	bool bLockOnDoubleTapPending = false;

	// True during the short parry window after starting a block
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Defense")
	bool bCanParry = false;

	// Prevents re-blocking until the cooldown expires
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Defense")
	bool bBlockOnCooldown = false;

	// How long to stun an attacker if parried
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Defense")
	float ParryStunDuration = 5.0f;

	UFUNCTION()
	void ClearParry();

	UFUNCTION()
	void ClearBlockCooldown();

	// Multiplier applied to incoming damage while blocking (0.0 = full block, 1.0 = no block)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Defense")
	float BlockDamageMultiplier = 0.35f;

	UFUNCTION()
	void RecoverFromStun();

	FTimerHandle ParryTimerHandle;
	FTimerHandle LockOnTapTimerHandle;
	FTimerHandle BlockCooldownTimerHandle;
	FTimerHandle StunTimerHandle;

protected:
	void Move(const FInputActionValue& Value);
	void LookX(const FInputActionValue& Value);
	void LookY(const FInputActionValue& Value);
	void Zoom(const FInputActionValue& Value);

	void UpdateCamera(float DeltaTime);
	void UpdateAnimation();
	void UpdateAttackSwing(float DeltaTime);
	void SetFlipbookForState(bool bIsMoving, EHybridSpriteDirection Direction);
};