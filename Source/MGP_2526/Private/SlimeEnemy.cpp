// Fill out your copyright notice in the Description page of Project Settings.

#include "SlimeEnemy.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ASlimeEnemy::ASlimeEnemy()
{
    bRotateSpriteWithCamera = true;
    EnemyAttackDamage = SlimeDamage;
    EnemyAttackWindupTime = SlimeAttackWindupTime;
    EnemyAttackRecoveryTime = SlimeAttackRecoveryTime;
    EnemyWalkSpeed = SlimeWalkSpeed;
    DetectionRange = SlimeDetectionRange;
    AttackRange = SlimeAttackRange;

    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = EnemyWalkSpeed;
    }

    static ConstructorHelpers::FObjectFinder<UPaperFlipbook> WalkLeftObj(TEXT("/Game/PaperAssets/Enemy/Slime/WalkLeft.WalkLeft"));
    static ConstructorHelpers::FObjectFinder<UPaperFlipbook> WalkRightObj(TEXT("/Game/PaperAssets/Enemy/Slime/WalkRight.WalkRight"));
    static ConstructorHelpers::FObjectFinder<UPaperFlipbook> WalkUpObj(TEXT("/Game/PaperAssets/Enemy/Slime/WalkUp.WalkUp"));
    static ConstructorHelpers::FObjectFinder<UPaperFlipbook> WalkDownObj(TEXT("/Game/PaperAssets/Enemy/Slime/WalkDown.WalkDown"));
    static ConstructorHelpers::FObjectFinder<UPaperFlipbook> AttackFrontObj(TEXT("/Game/PaperAssets/Enemy/Slime/AttackFront.AttackFront"));

    if (WalkLeftObj.Succeeded())
    {
        SlimeWalkLeftFlipbook = WalkLeftObj.Object;
    }
    if (WalkRightObj.Succeeded())
    {
        SlimeWalkRightFlipbook = WalkRightObj.Object;
    }
    if (WalkUpObj.Succeeded())
    {
        SlimeWalkUpFlipbook = WalkUpObj.Object;
    }
    if (WalkDownObj.Succeeded())
    {
        SlimeWalkDownFlipbook = WalkDownObj.Object;
    }
    if (AttackFrontObj.Succeeded())
    {
        SlimeAttackFrontFlipbook = AttackFrontObj.Object;
    }
}

void ASlimeEnemy::BeginPlay()
{
    Super::BeginPlay();

    // Make sure the slime actually threatens the player.
    EnemyAttackDamage = SlimeDamage;
    EnemyAttackWindupTime = SlimeAttackWindupTime;
    EnemyAttackRecoveryTime = SlimeAttackRecoveryTime;
    EnemyWalkSpeed = SlimeWalkSpeed;
    DetectionRange = SlimeDetectionRange;
    AttackRange = SlimeAttackRange;

    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = EnemyWalkSpeed;
    }

    FindTargetPlayer();

    if (Sprite)
    {
        SetSlimeFlipbook(GetWalkFlipbookForFacing(SlimeFacingDirection));
    }
}

void ASlimeEnemy::Tick(float DeltaTime)
{
    if (TargetPlayer)
    {
        CameraYaw = TargetPlayer->GetCameraYaw();
    }

    AEnemyCharacter::Tick(DeltaTime);
    UpdateSlimeAnimation();
}

void ASlimeEnemy::FindTargetPlayer()
{
    TargetPlayer = Cast<AHybridSpriteCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
}

void ASlimeEnemy::UpdateSlimeCombat(float DeltaTime)


{
    if (!IsValid(TargetPlayer))
    {
        FindTargetPlayer();
        AIState = EEnemyAIState::Idle;
        SetCombatState(ECombatState::Idle);
        return;
    }

    const float DistanceToPlayer = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());

    switch (AIState)
    {
    case EEnemyAIState::Idle:
        SetCombatState(ECombatState::Idle);
        if (DistanceToPlayer <= DetectionRange)
        {
            AIState = EEnemyAIState::Chasing;
        }
        break;

    case EEnemyAIState::Chasing:
        SetCombatState(ECombatState::Moving);
        if (DistanceToPlayer <= AttackRange)
        {
            StartSlimeAttack();
        }
        else
        {
            if (GetCharacterMovement())
            {
                GetCharacterMovement()->MaxWalkSpeed = EnemyWalkSpeed;
            }

            const FVector ToTarget = (TargetPlayer->GetActorLocation() - GetActorLocation());
            FVector FlatTarget = ToTarget;
            FlatTarget.Z = 0.0f;

            if (!FlatTarget.IsNearlyZero())
            {
                const FVector NormalizedTarget = FlatTarget.GetSafeNormal();
                AddMovementInput(NormalizedTarget, 1.0f);

                if (FMath::Abs(NormalizedTarget.X) > FMath::Abs(NormalizedTarget.Y))
                {
                    SlimeFacingDirection = NormalizedTarget.X < 0.0f ? ESlimeFacingDirection::Right : ESlimeFacingDirection::Left;
                }
                else
                {
                    SlimeFacingDirection = NormalizedTarget.Y > 0.0f ? ESlimeFacingDirection::Down : ESlimeFacingDirection::Up;
                }
            }
        }
        break;

    case EEnemyAIState::AttackWindup:
        StateElapsed += DeltaTime;
        SetCombatState(ECombatState::Attacking);
        if (StateElapsed >= EnemyAttackWindupTime)
        {
            ApplySlimeAttackDamage();
            AIState = EEnemyAIState::Recovering;
            StateElapsed = 0.0f;
        }
        break;

    case EEnemyAIState::Recovering:
        RecoverFromSlimeAttack(DeltaTime);
        break;
    }
}

void ASlimeEnemy::UpdateSlimeAnimation()
{
    if (!Sprite)
    {
        return;
    }

    if (TargetPlayer)
    {
        FVector ToTarget = TargetPlayer->GetActorLocation() - GetActorLocation();
        ToTarget.Z = 0.0f;

        if (!ToTarget.IsNearlyZero())
        {
            const FVector NormalizedTarget = ToTarget.GetSafeNormal();
            if (FMath::Abs(NormalizedTarget.X) > FMath::Abs(NormalizedTarget.Y))
            {
					SlimeFacingDirection = NormalizedTarget.X < 0.0f ? ESlimeFacingDirection::Right : ESlimeFacingDirection::Left;
            }
            else
            {
					SlimeFacingDirection = NormalizedTarget.Y > 0.0f ? ESlimeFacingDirection::Down : ESlimeFacingDirection::Up;
            }
        }
    }
    else
    {
        const FVector Velocity = GetVelocity();
        FVector FlatVelocity(Velocity.X, Velocity.Y, 0.0f);
        if (!FlatVelocity.IsNearlyZero())
        {
            FlatVelocity.Normalize();
            if (FMath::Abs(FlatVelocity.X) > FMath::Abs(FlatVelocity.Y))
            {
					SlimeFacingDirection = FlatVelocity.X < 0.0f ? ESlimeFacingDirection::Right : ESlimeFacingDirection::Left;
            }
            else
            {
					SlimeFacingDirection = FlatVelocity.Y > 0.0f ? ESlimeFacingDirection::Down : ESlimeFacingDirection::Up;
            }
        }
    }

    UPaperFlipbook* Desired = nullptr;

    if (AIState == EEnemyAIState::AttackWindup || CombatState == ECombatState::Attacking)
    {
        Desired = SlimeAttackFrontFlipbook;
    }
    else
    {
        Desired = GetWalkFlipbookForFacing(SlimeFacingDirection);
    }

    SetSlimeFlipbook(Desired);
    UpdateStatusText();
}

void ASlimeEnemy::SetSlimeFlipbook(UPaperFlipbook* DesiredFlipbook)
{
    if (!Sprite || !DesiredFlipbook)
    {
        return;
    }

    if (Sprite->GetFlipbook() != DesiredFlipbook)
    {
        Sprite->SetFlipbook(DesiredFlipbook);
        Sprite->SetLooping(true);
        Sprite->PlayFromStart();
    }
    else if (!Sprite->IsPlaying())
    {
        Sprite->Play();
    }
}

UPaperFlipbook* ASlimeEnemy::GetWalkFlipbookForFacing(ESlimeFacingDirection Facing) const
{
    switch (Facing)
    {
    case ESlimeFacingDirection::Left:
        return SlimeWalkRightFlipbook ? SlimeWalkRightFlipbook : SlimeWalkLeftFlipbook;
    case ESlimeFacingDirection::Right:
        return SlimeWalkLeftFlipbook ? SlimeWalkLeftFlipbook : SlimeWalkRightFlipbook;
    case ESlimeFacingDirection::Down:
        return SlimeWalkUpFlipbook ? SlimeWalkUpFlipbook : (SlimeWalkDownFlipbook ? SlimeWalkDownFlipbook : SlimeWalkLeftFlipbook);
    default:
        return SlimeWalkDownFlipbook ? SlimeWalkDownFlipbook : (SlimeWalkUpFlipbook ? SlimeWalkUpFlipbook : (SlimeWalkLeftFlipbook ? SlimeWalkLeftFlipbook : SlimeWalkRightFlipbook));
    }
}

void ASlimeEnemy::StartSlimeAttack()
{
    AIState = EEnemyAIState::AttackWindup;
    StateElapsed = 0.0f;
    bDamageAppliedThisAttack = false;
    SetCombatState(ECombatState::Attacking);
}

void ASlimeEnemy::ApplySlimeAttackDamage()
{
    if (!TargetPlayer || bDamageAppliedThisAttack)
    {
        return;
    }

    bDamageAppliedThisAttack = true;
    UGameplayStatics::ApplyDamage(TargetPlayer, SlimeDamage, GetController(), this, UDamageType::StaticClass());

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.25f, FColor::Red, TEXT("Slime hit the player for 1 damage"));
    }
}

void ASlimeEnemy::RecoverFromSlimeAttack(float DeltaTime)
{
    StateElapsed += DeltaTime;
    SetCombatState(ECombatState::Idle);

    if (StateElapsed >= EnemyAttackRecoveryTime)
    {
        StateElapsed = 0.0f;
        AIState = EEnemyAIState::Chasing;
    }
}
