// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResourceBarWidget.generated.h"

class UProgressBar;
class UTextBlock;

/**
 * 
 */
UCLASS()
class CPP_HELLOUNREAL_API UResourceBarWidget : public UUserWidget
{
	GENERATED_BODY()

protected :
	virtual void NativePreConstruct() override;

protected :
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> Bar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor FillColor = FLinearColor(0.f, 1.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor BackgroundColor = FLinearColor(0.f, 1.f, 0.f, 0.2f);

protected :

#if WITH_EDITOR
	// #if ~ #endif 사이의 코드는 에디터 상에서만 존재
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public :
	UFUNCTION(BlueprintCallable)
	void UpdateResourceBar(float InCurrent, float InMax);

};
