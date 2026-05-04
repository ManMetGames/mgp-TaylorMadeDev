// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacter.h"
#include "SlimeEnemy.generated.h"

class UPaperFlipbook;

UCLASS()
class MGP_2526_API ASlimeEnemy : public AEnemyCharacter
{
    GENERATED_BODY()

public:
    ASlimeEnemy();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

protected:
    // Walk flipbook for the slime
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Animation")
    UPaperFlipbook* SlimeWalkFlipbook = nullptr;

    // Wander state
    FVector WanderDirection = FVector::ZeroVector;
    float WanderTimer = 0.0f;
    float WanderInterval = 2.0f;
    float WanderSpeed = 150.0f;

    void UpdateWander(float DeltaTime);
};
