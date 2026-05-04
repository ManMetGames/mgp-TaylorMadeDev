// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"

class AHybridSpriteCharacter;

UCLASS(BlueprintType, Blueprintable)
class MGP_2526_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Health Bar")
	void SetSourceCharacter(AHybridSpriteCharacter* InSourceCharacter);

	UFUNCTION(BlueprintPure, Category = "Health Bar")
	float GetHealthPercent() const;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Health Bar")
	TObjectPtr<AHybridSpriteCharacter> SourceCharacter;
};