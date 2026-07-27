// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"

class UTextBlock;
class UProgressBar;
class UStatComponent;

/**
 * 
 */
UCLASS()
class CPP_HELLOUNREAL_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	
	virtual void NativeConstruct() override;

protected :

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor HealthBarColor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> HealthTextBlock;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStatComponent> OwnerStatComponent;

protected:
	UFUNCTION(BlueprintCallable)
	void InitializeHealthBar();

	UFUNCTION(BlueprintCallable)
	void UpdateHealthProgressBar(float Current, float Max);
	
};
