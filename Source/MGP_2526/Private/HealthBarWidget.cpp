// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthBarWidget.h"
#include "HybridSpriteCharacter.h"

void UHealthBarWidget::SetSourceCharacter(AHybridSpriteCharacter* InSourceCharacter)
{
	SourceCharacter = InSourceCharacter;
}

float UHealthBarWidget::GetHealthPercent() const
{
	if (!IsValid(SourceCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("HealthBarWidget: SourceCharacter is not set. Returning 0 percent."));
		return 0.0f;
	}

	return SourceCharacter->GetHealthPercent();
}