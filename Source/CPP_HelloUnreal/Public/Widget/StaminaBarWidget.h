// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StaminaBarWidget.generated.h"

class UTextBlock;
class UProgressBar;
class UStatComponent;

/**
 * 
 */
UCLASS()
class CPP_HELLOUNREAL_API UStaminaBarWidget : public UUserWidget
{
	GENERATED_BODY()

protected :
	
	virtual void NativeConstruct() override;

protected :

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor StaminaBarColor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> StaminaTextBlock;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> StaminaBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStatComponent> OwnerStatComponent;
	
protected :
	UFUNCTION(BlueprintCallable)
	void InitializeStaminaBar();

	UFUNCTION(BlueprintCallable)
	void UpdateStaminaProgressBar(float Current, float Max);

};
