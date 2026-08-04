// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamagePopupWidget.generated.h"

class UTextBlock;

DECLARE_DELEGATE(FOnPopUpAnimationFinished);

/**
 * 
 */
UCLASS()
class CPP_HELLOUNREAL_API UDamagePopupWidget : public UUserWidget
{
	GENERATED_BODY()

protected :

	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

public :

	UFUNCTION(BlueprintCallable)
	void SetDamage(float InDamage);

	UFUNCTION(BlueprintCallable)
	void PlayPopupAnimation();

public :

	FOnPopUpAnimationFinished OnPopUpAnimationFinished;

protected :

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> DamageText;

	// Transient: 런타임에 생성되는 거라고 명시적으로 표시
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PopupAnimation;

};
