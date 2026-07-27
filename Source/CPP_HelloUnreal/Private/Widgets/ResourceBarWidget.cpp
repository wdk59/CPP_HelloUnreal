
// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ResourceBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UResourceBarWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	Bar->SetFillColorAndOpacity(FillColor);	// 채워지는 색상 변경
	
	FProgressBarStyle Style = Bar->GetWidgetStyle();	// 기존 위젯 스타일을 가져와서
	Style.BackgroundImage.TintColor = BackgroundColor;	// 수정하고 (배경의 틴트만 변경)
	Bar->SetWidgetStyle(Style);							// 새로 적용하기
}

#if WITH_EDITOR
void UResourceBarWidget::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// 프로퍼티 이름 가져오기
	FName TargetPropertyName = (PropertyChangedEvent.Property != nullptr) ?
		PropertyChangedEvent.Property->GetFName() : NAME_None;

	// 가져온 프로퍼티 이름이 UResourceBarWidget의 FillColor와 같은지 확인
	if (TargetPropertyName == GET_MEMBER_NAME_CHECKED(UResourceBarWidget, FillColor))
	{
		BackgroundColor = FillColor;
		BackgroundColor.A = 0.2f;
	}
}
#endif

void UResourceBarWidget::UpdateResourceBar(float InCurrent, float InMax)
{
	Bar->SetPercent(InCurrent / InMax);
	CurrentText->SetText(FText::AsNumber(FMath::FloorToInt(InCurrent)));
	MaxText->SetText(FText::AsNumber(FMath::FloorToInt(InMax)));
}
