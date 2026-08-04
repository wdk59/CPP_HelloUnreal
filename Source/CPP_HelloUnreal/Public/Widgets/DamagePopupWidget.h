// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamagePopupWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class CPP_HELLOUNREAL_API UDamagePopupWidget : public UUserWidget
{
	GENERATED_BODY()

public :

	UFUNCTION(BlueprintCallable)
	void SetDamage(float InDamage);

	UFUNCTION(BlueprintCallable)
	void PlayPopupAnimation();

protected :

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> DamageText;

	// Transient: 런타임에 생성되는 거라고 명시적으로 표시
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PopupAnimation;


	
};
