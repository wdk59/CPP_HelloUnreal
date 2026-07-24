// Fill out your copyright notice in the Description page of Project Settings.


#include "Test06/StaminaBarWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Test02/StatComponent.h"
#include "../Interface/StatInterface.h"

void UStaminaBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	APawn* PlayerPawn = GetOwningPlayerPawn();

	if (PlayerPawn && PlayerPawn->Implements<UStatInterface>())
	{
		if (IStatInterface* StatInterface = Cast<IStatInterface>(PlayerPawn))
			OwnerStatComponent = StatInterface->GetStatComponent();
	}

	InitializeStaminaBar();

}

void UStaminaBarWidget::InitializeStaminaBar()
{
	FSlateFontInfo FontInfo = StaminaTextBlock->GetFont();
	FontInfo.Size = 20;
	StaminaTextBlock->SetFont(FontInfo);

	StaminaBarColor = FLinearColor::Blue;
	StaminaBar->SetFillColorAndOpacity(StaminaBarColor);

	if (!OwnerStatComponent)
		return;
	
	OwnerStatComponent->OnStaminaChange.AddDynamic(
		this,
		&UStaminaBarWidget::UpdateStaminaProgressBar
	);

	float CurrentStamina = IHealthInterface::Execute_GetCurrentHealth(OwnerStatComponent);
	float MaxStamina = IHealthInterface::Execute_GetMaxHealth(OwnerStatComponent);

	UpdateStaminaProgressBar(CurrentStamina, MaxStamina);
}

void UStaminaBarWidget::UpdateStaminaProgressBar(float Current, float Max)
{
	FText StaminaText = FText::Format(
		FText::FromString(TEXT("{0} / {1}")),
		FText::AsNumber(FMath::RoundToInt(Current)),
		FText::AsNumber(FMath::RoundToInt(Max))
	);

	StaminaTextBlock->SetText(StaminaText);
	StaminaBar->SetPercent(Current / Max);
}
