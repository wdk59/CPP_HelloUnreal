// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyBase.h"
#include "Components/StatComponent.h"
#include "Components/WidgetComponent.h"
#include "Widgets/ResourceBarWidget.h"
#include "Interface/StatInterface.h"
#include "Interface/HealthInterface.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("Stat"));
	
	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBarWidgetComponent->SetupAttachment(GetRootComponent());
	HealthBarWidgetComponent->SetDrawSize(FVector2D(200.f, 50.f));
	HealthBarWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 125.f));

}

UStatComponent* AEnemyBase::GetStatComponent() const
{
	return StatComponent;
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	InitializeStat();
	
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	UE_LOG(LogTemp, Log, TEXT("아야"));

	if (StatComponent && StatComponent->Implements<UHealthInterface>())
	{
		if (DamageAmount > 0)
		{
			IHealthInterface::Execute_ReceiveDamage(StatComponent, DamageAmount);
		}
		else
		{
			IHealthInterface::Execute_RecoveryHealth(StatComponent, DamageAmount);
		}
	}

	return DamageAmount;
}

void AEnemyBase::InitializeStat()
{
	if (HealthBarWidgetComponent && StatComponent)
	{
		if (UResourceBarWidget* HealthBar = Cast<UResourceBarWidget>(HealthBarWidgetComponent->GetUserWidgetObject()))
		{
			StatComponent->OnHealthChange.AddDynamic(HealthBar, &UResourceBarWidget::UpdateResourceBar);

			HealthBar->UpdateResourceBar(
				IHealthInterface::Execute_GetCurrentHealth(StatComponent),
				IHealthInterface::Execute_GetMaxHealth(StatComponent)
			);

			HealthBar->SetResourceBarColor(FLinearColor::Red);
		}
	}
}
