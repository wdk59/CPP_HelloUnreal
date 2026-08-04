// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/DamagePopupWidget.h"
#include "Components/TextBlock.h"

void UDamagePopupWidget::SetDamage(float InDamage)
{
	// static_cast: 컴파일 타임에 결정 -> 성능 안 먹음
	// DamageText가 없는 경우에는 Bind에서부터 오류가 나기 때문에 유효성 검사 생략
	DamageText->SetText(FText::AsNumber(static_cast<int32>(InDamage)));
}

void UDamagePopupWidget::PlayPopupAnimation()
{
	PlayAnimation(PopupAnimation);
}
