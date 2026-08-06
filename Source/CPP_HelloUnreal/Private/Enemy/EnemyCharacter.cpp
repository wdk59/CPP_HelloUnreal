// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyCharacter.h"
#include "Components/StatComponent.h"
#include "Interface/HealthInterface.h"
#include "CommonHeader/ItemDropTable.h"

#include "CPP_HelloUnreal/CPP_HelloUnreal.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StatComp = CreateDefaultSubobject<UStatComponent>(TEXT("StatComp"));

	GetCapsuleComponent()->SetCollisionObjectType(ECC_Enemy);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UStatComponent* AEnemyCharacter::GetStatComponent() const
{
	return nullptr;
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(StatComp))
	{
		StatComp->OnDie.AddDynamic(this, &AEnemyCharacter::OnDie);
	}
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	return Damage;
}

void AEnemyCharacter::OnDie()
{
	UE_LOG(LogTemp, Log, TEXT("%s가 죽었습니다."), *this->GetName());
	ItemDropTable->GetRowMap();
}
