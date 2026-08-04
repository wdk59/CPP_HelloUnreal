// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamagePopupActor.generated.h"

class UWidgetComponent;
class UDamagePopupWidget;

UCLASS()
class CPP_HELLOUNREAL_API ADamagePopupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADamagePopupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected :

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> DamagePopupWidgetComponent;

private :

	TObjectPtr<UDamagePopupWidget> DamagePopupWidget = nullptr;

public :
	
	UFUNCTION(BlueprintCallable)
	void OnPopupStart(float InDamage);

};
