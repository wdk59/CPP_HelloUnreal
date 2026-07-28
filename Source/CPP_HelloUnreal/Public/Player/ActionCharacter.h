// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Interface/StatInterface.h"
#include "Interface/WeaponUserInterface.h"
#include "ActionCharacter.generated.h"

class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UStatComponent;
class UAnimMontage;
class UAnimNotifyState_SectionJump;
class AWeaponActor;

UCLASS()
class CPP_HELLOUNREAL_API AActionCharacter : public ACharacter, public IStatInterface, public IWeaponUserInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AActionCharacter();

	UFUNCTION(BlueprintCallable, Category = "Stat")
	virtual UStatComponent* GetStatComponent() const override;
	
	virtual void OnWeaponAttackState(bool bEnable) override;

	void SetSectionJumpNotify(UAnimNotifyState_SectionJump* InSectionJumpNotify);

	virtual FOnWeaponAttackStateChanged& GetWeaponAttackStateChangedDelegate() override {
		return OnOnWeaponAttackStateChanged;
	}

	virtual bool SetNewWeapon(AWeaponActor* InWeapon) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public :
	FOnWeaponAttackStateChanged OnOnWeaponAttackStateChanged;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> IA_Test;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> IA_Sprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> IA_Roll;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> IA_Attack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> CameraSpringArmComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> CameraComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStatComponent> StatComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> RollMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> AttackMontage = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Move")
	float DefaultWalkSpeed = 600.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Move")
	float SprintSpeed = DefaultWalkSpeed * 2;

	// 달리기에 필요한 초당 스태미너 코스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	float SprintStaminaCostPerSecond = 2.f;

	// 구르기에 필요한 스태미너 코스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	float RollStaminaCost = 30.f;

	// 공격에 필요한 스태미너 코스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	float AttackCost = 5.f;

	// 스태미너 사용 후 자동 회복에 걸리는 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	float StaminaAutoRecoveryCoolTime = 3.f;

	// 스태미너가 자동 회복될 때 타이머 틱 당 회복량
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	float StaminaAutoRecoveryPerTick = 1.f;

	// 스태미너가 자동 회복될 때 타이머 한 틱의 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	float StaminaAutoRecoveryInterval = 0.1f;

	// 현재 가지고 있는 무기
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<AWeaponActor> CurrentWeapon = nullptr;

private:
	UPROPERTY()
	TObjectPtr<UAnimInstance> AnimInstance = nullptr;

	bool bSprintMode = false;

	// 발생한 콤보 노티파이를 저장해 놓는 변수
	TWeakObjectPtr<UAnimNotifyState_SectionJump> SectionJumpNotify = nullptr;
	
	// 현재 콤보가 가능한지 확인하기 위한 변수
	bool bComboReady = false;

protected :
	void OnTestAction(const FInputActionValue& Value);

	void OnMoveInput(const FInputActionValue& Value);

	void OnSprintStart();
	void OnSprintEnd();

	void OnRollAction(const FInputActionValue& Value);

	void OnAttackAction(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


private:

	void SpendSprintStamina(float DeltaTime);

	void SectionJumpForCombo();	// 콤보용으로 섹션 점프하는 함수

};
