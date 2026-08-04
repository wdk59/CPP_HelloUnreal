// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/DamagePopupActor.h"
#include "Components/WidgetComponent.h"
#include "Widgets/DamagePopupWidget.h"
#include "Frameworks/Subsystem/ObjectPoolSubsystem.h"

// Sets default values
ADamagePopupActor::ADamagePopupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DamagePopupWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamagePopupWidget"));
	SetRootComponent(DamagePopupWidgetComponent);

	DamagePopupWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	//DamageWidgetComponent->SetCastShadow(false);	// 월드 스페이스일 때 그림자 발생 방지
	DamagePopupWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void ADamagePopupActor::UseFinish()
{
	UE_LOG(LogTemp, Log, TEXT("Use Finish"));

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UObjectPoolSubsystem* SubSystem = GameInstance->GetSubsystem<UObjectPoolSubsystem>();
		SubSystem->ReturnPool(this);
	}
}

void ADamagePopupActor::OnSpawn_Implementation()
{
	SetActorHiddenInGame(false);
	//SetActorTickEnabled();		// Tick 켜고 끔. 이 액터는 애초에 틱을 안 써서 처음부터 꺼둠.
	//SetActorEnableCollision();	// 콜리전 켜고 끔. 이 액터는 애초에 콜리전을 안 써서 처음부터 꺼둠.

	/*OnPopupStart(0.f);

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(
		TimerHandle,
		this,
		&ADamagePopupActor::OnReturn,
		LifeTime,
		false
	);*/
}

void ADamagePopupActor::OnReturn_Implementation()
{
	// 눈에서 안 보이는 데로 보낸 다음에 숨기기
	// 주의) 너무 멀리 보내면 엔진이 자체적으로 지워버림.
	SetActorLocation(FVector(0.f, 0.f, -10000.f));
	SetActorHiddenInGame(true);
}

// Called when the game starts or when spawned
void ADamagePopupActor::BeginPlay()
{
	Super::BeginPlay();

	//DamagePopupWidgetComponent->GetUserWidgetObject();	// GetWidget()과 구현 동일
	// DamagePopupWidgetComponent는 반드시 있어야 돼서 없으면 터지라는 의도로 유효성 검사 생략
	DamagePopupWidget = Cast<UDamagePopupWidget>(DamagePopupWidgetComponent->GetWidget());

	DamagePopupWidget->OnPopUpAnimationFinished.BindUObject(this, &ADamagePopupActor::UseFinish);
	
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
