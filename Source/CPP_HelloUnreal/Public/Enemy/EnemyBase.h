// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/StatInterface.h"
#include "Interface/WeaponUserInterface.h"
#include "EnemyBase.generated.h"

class UWidgetComponent;
class UStatComponent;
class AWeaponActor;

UCLASS()
class CPP_HELLOUNREAL_API AEnemyBase : public ACharacter, public IStatInterface, public IWeaponUserInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

	UFUNCTION(BlueprintCallable, Category = "Stat")
	virtual UStatComponent* GetStatComponent() const override;

	virtual void OnWeaponAttackState(bool Enable) override;

	virtual FOnWeaponAttackStateChanged& GetWeaponAttackStateChangedDelegate() override {
		return OnOnWeaponAttackStateChanged;
	}

	virtual bool SetNewWeapon(AWeaponActor* InWeapon) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public :
	
	FOnWeaponAttackStateChanged OnOnWeaponAttackStateChanged;

protected :
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	TObjectPtr<UStatComponent> StatComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	TObjectPtr<UWidgetComponent> HealthBarWidgetComponent = nullptr;

	// 현재 가지고 있는 무기
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<AWeaponActor> CurrentWeapon = nullptr;

protected :
	
	UFUNCTION(BlueprintCallable)
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

private :
	
	void InitializeStat();

};
