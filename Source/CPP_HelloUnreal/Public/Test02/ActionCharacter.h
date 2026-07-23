// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "ActionCharacter.generated.h"

class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UStatComponent;
class UAnimMontage;

UCLASS()
class CPP_HELLOUNREAL_API AActionCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AActionCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> IA_Test;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> IA_Sprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> IA_Roll;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> CameraSpringArmComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> CameraComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStatComponent> StatComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> RollMontage = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float DefaultWalkSpeed = 600.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float SprintSpeed = DefaultWalkSpeed * 2;

	// 달리기에 필요한 초당 스태미너 코스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float SprintStaminaCostPerSecond = 2.f;

	// 구르기에 필요한 스태미너 코스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float RollStaminaCost = 30.f;

	// 스태미너 사용 후 자동 회복에 걸리는 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaAutoRecoveryCoolTime = 3.f;

	// 스태미너가 자동 회복될 때 타이머 틱 당 회복량
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaAutoRecoveryPerTick = 1.f;

	// 스태미너가 자동 회복될 때 타이머 한 틱의 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaAutoRecoveryInterval = 0.1f;

private:
	UPROPERTY()
	TObjectPtr<UAnimInstance> AnimInstance = nullptr;

	bool bSprintMode = false;

protected :
	void OnTestAction(const FInputActionValue& Value);

	void OnMoveInput(const FInputActionValue& Value);

	void OnSprintStart();
	void OnSprintEnd();

	void OnRollAction(const FInputActionValue& Value);

private:

	void SpendSprintStamina(float DeltaTime);

};
