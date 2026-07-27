// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/ActionHUD.h"
#include "Blueprint/UserWidget.h"

void AActionHUD::BeginPlay()
{
	Super::BeginPlay();

	if (MainHudWidgetClass)
	{
		MainHudWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), MainHudWidgetClass);
		if (MainHudWidgetInstance)
		{
			MainHudWidgetInstance->AddToViewport();
		}
	}

}

UUserWidget* AActionHUD::GetMainHudWidget() const
{
	return MainHudWidgetInstance;
}
