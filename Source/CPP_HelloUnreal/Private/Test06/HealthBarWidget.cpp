// Fill out your copyright notice in the Description page of Project Settings.


#include "Test06/HealthBarWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Test02/StatComponent.h"
#include "../Interface/StatInterface.h"

void UHealthBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	APawn* PlayerPawn = GetOwningPlayerPawn();

	if (PlayerPawn && PlayerPawn->Implements<UStatInterface>())
	{
		if (IStatInterface* StatInterface = Cast<IStatInterface>(PlayerPawn))
		{
			OwnerStatComponent = StatInterface->GetStatComponent();
		}
	}

	InitializeHealthBar();

}

void UHealthBarWidget::InitializeHealthBar()
{
	HealthBarColor = FLinearColor::Red;
	HealthBar->SetFillColorAndOpacity(HealthBarColor);

	if (!OwnerStatComponent)
		return;

	OwnerStatComponent->OnHealthChange.AddDynamic(
		this,
		&UHealthBarWidget::UpdateHealthProgressBar
	);

	float CurrentHealth = IHealthInterface::Execute_GetCurrentHealth(OwnerStatComponent);
	float MaxHealth = IHealthInterface::Execute_GetMaxHealth(OwnerStatComponent);

	UpdateHealthProgressBar(CurrentHealth, MaxHealth);

}

void UHealthBarWidget::UpdateHealthProgressBar(float Current, float Max)
{
	//FText HealthText = FText::FromString(FString::Printf(TEXT("%.f / %.f"), Current, Max));
	FText HealthText = FText::Format(
		FText::FromString(TEXT("{0} / {1}")),
		FText::AsNumber(FMath::RoundToInt(Current)),
		FText::AsNumber(FMath::RoundToInt(Max))
	);

	HealthTextBlock->SetText(HealthText);
	HealthBar->SetPercent(Current / Max);

}