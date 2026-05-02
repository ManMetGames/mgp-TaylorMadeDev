// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "HybridSpriteCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UPaperFlipbookComponent;
class USceneComponent;
class UInputMappingContext;
class UInputAction;
class UPaperFlipbook;

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
	UInputAction* BlockAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* ParryAction;

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

protected:
	void Move(const FInputActionValue& Value);
	void LookX(const FInputActionValue& Value);
	void LookY(const FInputActionValue& Value);
	void Zoom(const FInputActionValue& Value);

	void UpdateCamera(float DeltaTime);
	void UpdateAnimation();
	void SetFlipbookForState(bool bIsMoving, EHybridSpriteDirection Direction);

public:
	// -------------------------
	// Combat State
	// -------------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|State")
	ECombatState CombatState = ECombatState::Idle;

	UFUNCTION(BlueprintCallable, Category = "Combat|State")
	void SetCombatState(ECombatState NewState);

	UFUNCTION(BlueprintCallable, Category = "Combat|State")
	bool IsInState(ECombatState StateToCheck) const;

	UFUNCTION(BlueprintCallable, Category = "Combat|State")
	bool CanAttack() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Combat|State")
	bool IsWeaponEquipped() const;

	// -------------------------
	// Attack Mechanics
	// -------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Attack")
	float AttackDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Attack")
	float AttackWindupTime = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Attack")
	float AttackActiveTime = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Attack")
	float AttackRecoveryTime = 0.3f;

	UFUNCTION(BlueprintCallable, Category = "Combat|Attack")
	void StartAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat|Attack")
	void BeginAttackActiveFrames();

	UFUNCTION(BlueprintCallable, Category = "Combat|Attack")
	void EndAttackActiveFrames();

	UFUNCTION(BlueprintCallable, Category = "Combat|Attack")
	void FinishAttack();

protected:
	FTimerHandle AttackWindupTimerHandle;
	FTimerHandle AttackActiveTimerHandle;
	FTimerHandle AttackRecoveryTimerHandle;

};