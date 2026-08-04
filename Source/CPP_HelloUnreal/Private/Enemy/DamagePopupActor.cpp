// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/DamagePopupActor.h"
#include "Components/WidgetComponent.h"
#include "Widgets/DamagePopupWidget.h"

// Sets default values
ADamagePopupActor::ADamagePopupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DamagePopupWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamagePopupWidget"));
	SetRootComponent(DamagePopupWidgetComponent);

	DamagePopupWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	//DamageWidgetComponent->SetCastShadow(false);	// 월드 스페이스일 때 그림자 발생 방지

}

// Called when the game starts or when spawned
void ADamagePopupActor::BeginPlay()
{
	Super::BeginPlay();

	//DamagePopupWidgetComponent->GetUserWidgetObject();	// GetWidget()과 구현 동일
	// DamagePopupWidgetComponent는 반드시 있어야 돼서 없으면 터지라는 의도로 유효성 검사 생략
	DamagePopupWidget = Cast<UDamagePopupWidget>(DamagePopupWidgetComponent->GetWidget());
	
}

// Called every frame
void ADamagePopupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADamagePopupActor::OnPopupStart(float InDamage)
{
	if (DamagePopupWidget)
	{
		DamagePopupWidget->SetDamage(InDamage);
		DamagePopupWidget->PlayPopupAnimation();
	}
}

