// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PlayerStatBarsWidget.h"
#include "Widgets/ResourceBarWidget.h"
#include "Interface/StatInterface.h"
#include "Interface/HealthInterface.h"
#include "Interface/StaminaInterface.h"
#include "Components/StatComponent.h"

void UPlayerStatBarsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	InitializePlayerStatBars();
}

void UPlayerStatBarsWidget::InitializePlayerStatBars()
{
	if (IStatInterface* Player = Cast<IStatInterface>(GetOwningPlayerPawn()))
	{
		if (UStatComponent* Stat = Player->GetStatComponent())
		{
			Stat->OnHealthChange.AddDynamic(HealthBar, &UResourceBarWidget::UpdateResourceBar);
			Stat->OnStaminaChange.AddDynamic(StaminaBar, &UResourceBarWidget::UpdateResourceBar);

			HealthBar->UpdateResourceBar(
				IHealthInterface::Execute_GetCurrentHealth(Stat),
				IHealthInterface::Execute_GetMaxHealth(Stat)
			);
			StaminaBar->UpdateResourceBar(
				IStaminaInterface::Execute_GetCurrentStamina(Stat),
				IStaminaInterface::Execute_GetMaxStamina(Stat)
			);
		}
	}
}
