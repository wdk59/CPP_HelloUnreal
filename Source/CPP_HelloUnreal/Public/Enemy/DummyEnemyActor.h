// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DummyEnemyActor.generated.h"

class ADamagePopupActor;
class ABloodPopupActor;

UCLASS()
class CPP_HELLOUNREAL_API ADummyEnemyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADummyEnemyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

protected :
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> PopupTransform = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> BloodTransform = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ADamagePopupActor> DamagePopupClass = nullptr;

	TSubclassOf<ABloodPopupActor> BloodPopupClass = nullptr;

};
