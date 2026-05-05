// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyCharacter.h"

#include "Components/WidgetComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HealthBarWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"

AEnemyCharacter::AEnemyCharacter()
{
	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	EnemyStatusText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("EnemyStatusText"));
	EnemyStatusText->SetupAttachment(GetRootComponent());
	EnemyStatusText->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
	EnemyStatusText->SetHorizontalAlignment(EHTA_Center);
	EnemyStatusText->SetVerticalAlignment(EVRTA_TextCenter);
	EnemyStatusText->SetWorldSize(24.0f);
	EnemyStatusText->SetTextRenderColor(FColor::White);

	EnemyAttackDamage = AttackDamage;
	EnemyAttackWindupTime = AttackWindupTime;
	EnemyAttackRecoveryTime = AttackRecoveryTime;
	EnemyWalkSpeed = 280.0f;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = EnemyWalkSpeed;
	}

	MaxHealth = 60.0f;
	CurrentHealth = MaxHealth;

	static ConstructorHelpers::FClassFinder<UUserWidget> HealthBarWidgetClass(TEXT("/Game/UI/WBP_HealthBar"));
	if (HealthBarWidgetClass.Succeeded() && HealthBarWidget)
	{
		HealthBarWidget->SetWidgetClass(HealthBarWidgetClass.Class);
	}
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = EnemyWalkSpeed;
	}

	BindHealthBarToSelf();
	FindTarget();
	UpdateStatusText();
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateEnemyAI(DeltaTime);
	UpdateStatusText();
}

void AEnemyCharacter::BindHealthBarToSelf()
{
	if (!HealthBarWidget)
	{
		return;
	}

	HealthBarWidget->InitWidget();

	if (UHealthBarWidget* HealthWidget = Cast<UHealthBarWidget>(HealthBarWidget->GetUserWidgetObject()))
	{
		HealthWidget->SetSourceCharacter(this);
	}
	else if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow, TEXT("Enemy: health bar widget class is not UHealthBarWidget."));
	}
}

void AEnemyCharacter::FindTarget()
{
	TargetPlayer = Cast<AHybridSpriteCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
}

void AEnemyCharacter::UpdateEnemyAI(float DeltaTime)
{
    if (!IsValid(TargetPlayer))
	{
		FindTarget();
		AIState = EEnemyAIState::Idle;
		return;
	}

	if (AIState == EEnemyAIState::Stunned)
	{
		SetCombatState(ECombatState::Stunned);
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->StopMovementImmediately();
			GetCharacterMovement()->DisableMovement();
		}
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
			StartAttackWindup();
		}
		else
		{
			ChaseTarget(DeltaTime);
		}
		break;

	case EEnemyAIState::AttackWindup:
		StateElapsed += DeltaTime;
		SetCombatState(ECombatState::Attacking);
		if (StateElapsed >= EnemyAttackWindupTime)
		{
			ApplyAttackDamage();
			AIState = EEnemyAIState::Recovering;
			StateElapsed = 0.0f;
		}
		break;

	case EEnemyAIState::Recovering:
		RecoverFromAttack(DeltaTime);
		break;
	}
}

void AEnemyCharacter::UpdateStatusText()
{
	if (!EnemyStatusText)
	{
		return;
	}

	const bool bAttacking = AIState == EEnemyAIState::AttackWindup || CombatState == ECombatState::Attacking;
	const bool bCanBeParried = AIState == EEnemyAIState::AttackWindup;
	const FString StateText =
		AIState == EEnemyAIState::Idle ? TEXT("Idle") :
		AIState == EEnemyAIState::Chasing ? TEXT("Chasing") :
		AIState == EEnemyAIState::Stunned ? TEXT("Stunned") :
		AIState == EEnemyAIState::AttackWindup ? TEXT("AttackWindup") :
		TEXT("Recovering");

	const FString PromptText = bCanBeParried ? TEXT("\nParry now!") : TEXT("");

	const FString Status = FString::Printf(
		TEXT("Health: %.0f/%.0f\nAttacking: %s\nState: %s%s"),
		CurrentHealth,
		MaxHealth,
		bAttacking ? TEXT("True") : TEXT("False"),
		*StateText,
		*PromptText
	);

	EnemyStatusText->SetText(FText::FromString(Status));

	// Make the status text face the player so it's always readable
	if (TargetPlayer)
	{
		const FVector TextLocation = EnemyStatusText->GetComponentLocation();
		const FVector ToPlayer = TargetPlayer->GetActorLocation() - TextLocation;
		if (!ToPlayer.IsNearlyZero())
		{
			const FRotator LookAt = ToPlayer.Rotation();
			// Reverse the previous orientation so the text reads the other way.
			EnemyStatusText->SetWorldRotation(FRotator(0.0f, LookAt.Yaw, 0.0f));
		}
	}
}

void AEnemyCharacter::ChaseTarget(float DeltaTime)
{
	if (!TargetPlayer)
	{
		return;
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = EnemyWalkSpeed;
	}

	const FVector ToTarget = (TargetPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	AddMovementInput(ToTarget, 1.0f);
}

void AEnemyCharacter::StartAttackWindup()
{
	AIState = EEnemyAIState::AttackWindup;
	StateElapsed = 0.0f;
	bDamageAppliedThisAttack = false;
	SetCombatState(ECombatState::Attacking);
}

void AEnemyCharacter::ApplyAttackDamage()
{
	if (!TargetPlayer || bDamageAppliedThisAttack)
	{
		return;
	}

	bDamageAppliedThisAttack = true;
	UGameplayStatics::ApplyDamage(TargetPlayer, EnemyAttackDamage, GetController(), this, UDamageType::StaticClass());

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.25f, FColor::Red, TEXT("Enemy hit the player"));
	}
}

void AEnemyCharacter::RecoverFromAttack(float DeltaTime)
{
	StateElapsed += DeltaTime;
	SetCombatState(ECombatState::Idle);

	if (StateElapsed >= EnemyAttackRecoveryTime)
	{
		StateElapsed = 0.0f;
		AIState = EEnemyAIState::Chasing;
	}
}

void AEnemyCharacter::Stun(float Duration)
{
	// Cancel other AI state timers and enter stunned state
	AIState = EEnemyAIState::Stunned;
	SetCombatState(ECombatState::Stunned);
	SetActorTickEnabled(false);

	// Stop movement immediately
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();
	}

	UpdateStatusText();

	// Set a timer to clear stun
	GetWorldTimerManager().ClearTimer(StunTimerHandle);
	GetWorldTimerManager().SetTimer(StunTimerHandle, this, &AEnemyCharacter::ClearStun, Duration, false);
}

void AEnemyCharacter::ClearStun()
{
	AIState = EEnemyAIState::Idle;
	SetCombatState(ECombatState::Idle);
	SetActorTickEnabled(true);

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}