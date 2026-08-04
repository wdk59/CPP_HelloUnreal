// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/PoolableInterface.h"
#include "BloodPopupActor.generated.h"

class UNiagaraComponent;

UCLASS()
class CPP_HELLOUNREAL_API ABloodPopupActor : public AActor, public IPoolableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABloodPopupActor();

	UFUNCTION(BlueprintCallable)
	virtual void UseFinish() override;

	virtual void OnSpawn_Implementation() override;

	virtual void OnReturn_Implementation() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected :

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> BloodVFXComponent = nullptr;

protected :
	
	UFUNCTION(BlueprintCallable)
	void OnBloodVFXFinished(UNiagaraComponent* FinishedComponent);

};
