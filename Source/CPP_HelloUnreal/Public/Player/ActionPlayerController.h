// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "ActionPlayerController.generated.h"

/**
 * 
 */

class UInputMappingContext;
class UInputAction;

UCLASS()
class CPP_HELLOUNREAL_API AActionPlayerController : public APlayerController
{
	GENERATED_BODY()

protected :
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private :
	// 입력 우선 순위
	int32 GameInputPriority = 0;	// 우선순위를 가장 높게 지정하겠다: 플레이어기 때문

protected :
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputMappingContext> DefaultMappingContext = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> IA_Look;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ViewPitchMax = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ViewPitchMin = -40.f;

protected :
	void OnLookInput(const FInputActionValue& InValue);
	
};
