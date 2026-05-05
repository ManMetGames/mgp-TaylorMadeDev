// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HybridSpriteCharacter.h"
#include "EnemyCharacter.generated.h"

class AHybridSpriteCharacter;
class UTextRenderComponent;

UENUM(BlueprintType)
enum class EEnemyAIState : uint8
{
	Idle,
	Chasing,
	Stunned,
	AttackWindup,
	Recovering
};

UCLASS()
class MGP_2526_API AEnemyCharacter : public AHybridSpriteCharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

	// Stun the enemy for the given duration (used by parry)
	UFUNCTION(BlueprintCallable, Category = "Enemy|State")
	void Stun(float Duration);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|AI")
	EEnemyAIState AIState = EEnemyAIState::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|AI")
	AHybridSpriteCharacter* TargetPlayer = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|UI")
	UTextRenderComponent* EnemyStatusText = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|AI")
	float DetectionRange = 2200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|AI")
	float AttackRange = 140.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|AI")
	float EnemyWalkSpeed = 280.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|AI")
	float EnemyAttackDamage = 14.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|AI")
	float EnemyAttackWindupTime = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|AI")
	float EnemyAttackRecoveryTime = 0.5f;

	float StateElapsed = 0.0f;
	bool bDamageAppliedThisAttack = false;

	FTimerHandle StunTimerHandle;

	void ClearStun();

	void FindTarget();
	void UpdateEnemyAI(float DeltaTime);
	void ChaseTarget(float DeltaTime);
	void StartAttackWindup();
	void ApplyAttackDamage();
	void RecoverFromAttack(float DeltaTime);
	void BindHealthBarToSelf();
	void UpdateStatusText();
};