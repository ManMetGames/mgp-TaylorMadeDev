// Fill out your copyright notice in the Description page of Project Settings.

#include "SlimeEnemy.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"
#include "Components/TextRenderComponent.h"

ASlimeEnemy::ASlimeEnemy()
{
    // Make sure this enemy doesn't try to attack the player by default
    AttackRange = 0.0f;
    DetectionRange = 0.0f;
    EnemyAttackDamage = 0.0f;

    WanderInterval = 2.0f;
    WanderSpeed = 120.0f;
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = WanderSpeed;
    }

    // Attempt to find a SlimeWalk flipbook asset. Adjust path if your asset lives elsewhere.
    static ConstructorHelpers::FObjectFinder<UPaperFlipbook> FlipbookObj(TEXT("/Game/PaperAssets/Enemy/Slime/SlimeWalk.SlimeWalk"));
    if (FlipbookObj.Succeeded())
    {
        SlimeWalkFlipbook = FlipbookObj.Object;
    }
}

void ASlimeEnemy::BeginPlay()
{
    Super::BeginPlay();

    // Ensure we start with the slime walk animation if available
    if (Sprite && SlimeWalkFlipbook)
    {
        Sprite->SetFlipbook(SlimeWalkFlipbook);
    }

    // initialize wander timer so we pick a direction immediately
    WanderTimer = 0.0f;
}

void ASlimeEnemy::Tick(float DeltaTime)
{
    // Call the hybrid sprite tick to update animations and camera bob, but skip Enemy AI
    AHybridSpriteCharacter::Tick(DeltaTime);

    UpdateWander(DeltaTime);

    // Keep status text updated and facing the player
    UpdateStatusText();
}

void ASlimeEnemy::UpdateWander(float DeltaTime)
{
    if (WanderTimer <= 0.0f || WanderDirection.IsNearlyZero())
    {
        // Pick a new random horizontal direction
        WanderDirection = FVector(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f), 0.0f).GetSafeNormal();
        WanderInterval = FMath::FRandRange(1.2f, 3.0f);
        WanderTimer = WanderInterval;
    }

    WanderTimer -= DeltaTime;

    // Move the slime along the chosen direction
    if (!WanderDirection.IsNearlyZero())
    {
        if (GetCharacterMovement())
        {
            GetCharacterMovement()->MaxWalkSpeed = WanderSpeed;
        }
        AddMovementInput(WanderDirection, 1.0f);
        SetCombatState(ECombatState::Moving);

        if (Sprite && SlimeWalkFlipbook)
        {
            Sprite->SetFlipbook(SlimeWalkFlipbook);
        }
    }
    else
    {
        SetCombatState(ECombatState::Idle);
    }
}
