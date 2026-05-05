// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacter.h"
#include "SlimeEnemy.generated.h"

class UPaperFlipbook;

UENUM(BlueprintType)
enum class ESlimeFacingDirection : uint8
{
    Left,
    Right,
    Down,
    Up
};

UCLASS()
class MGP_2526_API ASlimeEnemy : public AEnemyCharacter
{
    GENERATED_BODY()

public:
    ASlimeEnemy();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

protected:
    // Walk/attack flipbooks for the slime
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Animation")
    UPaperFlipbook* SlimeWalkLeftFlipbook = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Animation")
    UPaperFlipbook* SlimeWalkRightFlipbook = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Animation")
    UPaperFlipbook* SlimeWalkUpFlipbook = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Animation")
    UPaperFlipbook* SlimeWalkDownFlipbook = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Animation")
    UPaperFlipbook* SlimeAttackFrontFlipbook = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Combat")
    float SlimeDamage = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Combat")
    float SlimeWalkSpeed = 210.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Combat")
    float SlimeDetectionRange = 1600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Combat")
    float SlimeAttackRange = 140.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Combat")
    float SlimeAttackWindupTime = 0.45f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Combat")
    float SlimeAttackRecoveryTime = 0.45f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Combat")
    float SlimeParryWindow = 0.18f;

    ESlimeFacingDirection SlimeFacingDirection = ESlimeFacingDirection::Up;

    void FindTargetPlayer();
    void UpdateSlimeCombat(float DeltaTime);
    void UpdateSlimeAnimation();
    void SetSlimeFlipbook(UPaperFlipbook* DesiredFlipbook);
    UPaperFlipbook* GetWalkFlipbookForFacing(ESlimeFacingDirection Facing) const;
    void StartSlimeAttack();
    void ApplySlimeAttackDamage();
    void RecoverFromSlimeAttack(float DeltaTime);
};
