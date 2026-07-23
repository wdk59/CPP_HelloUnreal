// Fill out your copyright notice in the Description page of Project Settings.


#include "Test02/StatComponent.h"

// Sets default values for this component's properties
UStatComponent::UStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UStatComponent::InitializeStat(FAutoRecoveryData& InData)
{
	CurrentStamina = MaxStamina;
	StaminaRecoveryData = InData;
}

float UStatComponent::GetCurrentStamina_Implementation() const
{
	return CurrentStamina;
}

bool UStatComponent::ConsumeStamina_Implementation(float InAmount)
{
	bool bResult = false;

	if (CurrentStamina >= InAmount)
	{
		CurrentStamina -= InAmount;

		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.SetTimer(
			StaminaAutoRecoveryTimerHandle,
			this,
			&UStatComponent::StaminaAutoRecoveryPerTick,
			StaminaRecoveryData.TickInterval,
			true,
			StaminaRecoveryData.CoolTime
		);

		bResult = true;
	}

	UE_LOG(LogTemp, Log, TEXT("Stamina: %.1f / %.1f"), CurrentStamina, MaxStamina);
	return bResult;
}

void UStatComponent::RecoveryStamina_Implementation(float InAmount)
{
	CurrentStamina = FMath::Clamp(CurrentStamina + InAmount, 0, MaxStamina);
	UE_LOG(LogTemp, Log, TEXT("Stamina: %.1f / %.1f"), CurrentStamina, MaxStamina);

	// 실수 비교 찝찝하면~
	//FMath::IsNearlyEqual(CurrentStamina, MaxStamina)
	if (CurrentStamina >= MaxStamina)
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.ClearTimer(StaminaAutoRecoveryTimerHandle);
	}
}

float UStatComponent::GetCurrentHealth_Implementation() const
{
	return CurrentHealth;
}

bool UStatComponent::ReceiveDamage_Implementation(float InAmount)
{
	bool bAlive = true;

	CurrentHealth = FMath::Clamp(CurrentHealth - InAmount, 0, MaxHealth);
	
	if (CurrentHealth <= 0)
	{
		bAlive = false;
	}

	UE_LOG(LogTemp, Log, TEXT("Health: %.1f / %.1f"), CurrentHealth, MaxHealth);
	return bAlive;
}

void UStatComponent::RecoveryHealth_Implementation(float InAmount)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + InAmount, 0, MaxHealth);
	UE_LOG(LogTemp, Log, TEXT("Health: %.1f / %.1f"), CurrentHealth, MaxHealth);
}

// Called when the game starts
void UStatComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentStamina = MaxStamina;
	
}


// Called every frame
void UStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UStatComponent::StaminaAutoRecoveryPerTick()
{
	IStaminaInterface::Execute_RecoveryStamina(this, StaminaRecoveryData.RecoveryPerTick);
}

