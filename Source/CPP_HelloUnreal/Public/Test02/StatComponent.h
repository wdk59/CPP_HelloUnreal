// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Interface/StaminaInterface.h"
#include "../Interface/HealthInterface.h"
#include "StatComponent.generated.h"

//DECLARE_DELEGATE(FNormal);	// 함수 포인터 사용, 빠르다.
//DECLARE_DYNAMIC_DELEGATE(FDynamic);	// 리플랙션 시스템 사용(문자열을 키로 하는 테이블), 느리다.
//
//DECLARE_MULTICAST_DELEGATE(FMulticast);	// 바인딩 시킬 함수를 여러개 저장할 수 있다.
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicMulticast);
//
//DECLARE_MULTICAST_DELEGATE_TwoParams(Fmulticast2Param, int, float);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynamicMultiParam, int, Data);
//
//DECLARE_DELEGATE_RetVal(int, FNormalReturn);	// 리턴을 int로 하는 델리게이트
//DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(float, FDynamicReturn, int, Data);	// 리턴이 float이고 파라미터가 int Data인 델리게이트
//
// DYNAMIC 델리게이트에 바인딩하는 함수는 UFUNCTION이 반드시 붙어 있어야 한다. (람다식 사용 불가)
// 
// Evenet
// - 델리게이트와 거의 같음
// - 기본적으로 멀티캐스트(리스너가 여러명)
// - 리턴값이 무조건 없다.
// - 클래스 밖에서 호출할 수 없다.
// - C++ 전용
//DECLARE_EVENT(AActor, FTestEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatEmpty);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatChange, float, Current, float, Max);

struct FAutoRecoveryData
{
	float CoolTime = 3.f;			// 자동 회복이 있을 때까지 걸릴 시간
	float TickInterval = 0.1f;		// 한 틱의 시간
	float RecoveryPerTick = 1.f;	// 틱당 회복량

	FAutoRecoveryData() : CoolTime(1.f), TickInterval(1.f), RecoveryPerTick(1.f)
	{
	};
	FAutoRecoveryData(float InCoolTime, float InTickInterval, float InRecoveryPerTick)
		: CoolTime(InCoolTime), TickInterval(InTickInterval), RecoveryPerTick(InRecoveryPerTick)
	{
	};

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CPP_HELLOUNREAL_API UStatComponent : public UActorComponent, public IStaminaInterface, public IHealthInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatComponent();

	void InitializeStat(FAutoRecoveryData& InData);

	virtual float GetCurrentStamina_Implementation() const override;
	virtual float GetMaxStamina_Implementation() const override;
	virtual bool ConsumeStamina_Implementation(float InAmount) override;
	virtual void RecoveryStamina_Implementation(float InAmount) override;

	virtual float GetCurrentHealth_Implementation() const override;
	virtual float GetMaxHealth_Implementation() const override;
	virtual void ReceiveDamage_Implementation(float InAmount) override;
	virtual void RecoveryHealth_Implementation(float InAmount) override;

public :
	UPROPERTY(BlueprintAssignable, Category = "Stat|Stamina")
	FOnStatEmpty OnStaminaEmpty;
	UPROPERTY(BlueprintAssignable, Category = "Stat|Health")
	FOnStatEmpty OnDie;
	UPROPERTY(BlueprintAssignable, Category = "Stat|Stamina")
	FOnStatChange OnStaminaChange;
	UPROPERTY(BlueprintAssignable, Category = "Stat|Health")
	FOnStatChange OnHealthChange;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected :

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth = 100.0f;

private :
	// 스태미너 자동 회복 처리를 위한 타이머
	FTimerHandle StaminaAutoRecoveryTimerHandle;

	// 스태미너 자동 회복용 데이터
	FAutoRecoveryData StaminaRecoveryData;

	const float EmptyCheckLimit = 0.01f;
		
private :
	
	void StaminaAutoRecoveryPerTick();

};
