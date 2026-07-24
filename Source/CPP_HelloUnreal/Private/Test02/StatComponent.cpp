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

float UStatComponent::GetMaxStamina_Implementation() const
{
	return MaxStamina;
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

		// 블루프린트 디스패처 Call과 같다.
		OnStaminaChange.Broadcast(CurrentStamina, MaxStamina);

		bResult = true;
	}

	if (CurrentStamina < EmptyCheckLimit)
	{
		OnStaminaEmpty.Broadcast();
	}

	UE_LOG(LogTemp, Log, TEXT("Stamina: %.1f / %.1f"), CurrentStamina, MaxStamina);
	return bResult;
}

void UStatComponent::RecoveryStamina_Implementation(float InAmount)
{
	CurrentStamina = FMath::Min(CurrentStamina + InAmount, MaxStamina);
	OnStaminaChange.Broadcast(CurrentStamina, MaxStamina);
	
	//UE_LOG(LogTemp, Log, TEXT("Stamina: %.1f / %.1f"), CurrentStamina, MaxStamina);

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

float UStatComponent::GetMaxHealth_Implementation() const
{
	return MaxHealth;
}

void UStatComponent::ReceiveDamage_Implementation(float InAmount)
{

	CurrentHealth = FMath::Max(CurrentHealth - InAmount, 0);
	//CurrentHealth -= InAmount;
	OnHealthChange.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0.f)
	{
		CurrentHealth = 0;
		OnDie.Broadcast();
	}

	//UE_LOG(LogTemp, Log, TEXT("Health: %.1f / %.1f"), CurrentHealth, MaxHealth);

}

void UStatComponent::RecoveryHealth_Implementation(float InAmount)
{
	CurrentHealth = FMath::Min(CurrentHealth + InAmount, MaxHealth);
	OnHealthChange.Broadcast(CurrentHealth, MaxHealth);
	//UE_LOG(LogTemp, Log, TEXT("Health: %.1f / %.1f"), CurrentHealth, MaxHealth);
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

