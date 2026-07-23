// Fill out your copyright notice in the Description page of Project Settings.


#include "Test02/ActionCharacter.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Test02/StatComponent.h"
#include "Animation/AnimMontage.h"

// Sets default values
AActionCharacter::AActionCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArmComponent->SetupAttachment(RootComponent);
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(CameraSpringArmComponent);
	CameraSpringArmComponent->bUsePawnControlRotation = true;	// 스프링암은 컨트롤러 입력대로 회전
	
	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("Stat"));

	bUseControllerRotationYaw = false;	// 컨트롤러 움직일 때 폰이 같이 회전되는 것 방지
	GetCharacterMovement()->bOrientRotationToMovement = true;	// 캐릭터 이동 방향으로 바라보게 만들기

}

// Called when the game starts or when spawned
void AActionCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	}
	if (GetMesh())
	{
		AnimInstance = GetMesh()->GetAnimInstance();
	}

	bSprintMode = false;

	//GetCurrentStamina();	// 실행했을 때 C++에 구현된 내용만 호출
	//IStaminaInterface::Execute_GetCurrentStamina(this);	// 실행했을 때 블루프린트 구현으로 호출

	if (StatComponent)
	{
		FAutoRecoveryData Data = FAutoRecoveryData(
			StaminaAutoRecoveryCoolTime,
			StaminaAutoRecoveryInterval,
			StaminaAutoRecoveryPerTick
		);
		StatComponent->InitializeStat(Data);
	}
}

// Called every frame
void AActionCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpendSprintStamina(DeltaTime);

}

void AActionCharacter::SpendSprintStamina(float DeltaTime)
{
	// 달리기 모드이고, 이동하고 있고, 몽타주 재생 중이 아니면
	if (bSprintMode && !GetVelocity().IsNearlyZero() &&
		AnimInstance && !AnimInstance->IsAnyMontagePlaying())
	{
		// 스태미너 지속 감소
		if (!IStaminaInterface::Execute_ConsumeStamina(StatComponent, SprintStaminaCostPerSecond * DeltaTime))
		{
			OnSprintEnd();	// 스태미너가 다 떨어지면 달리기 모드 정지
		}
	}
}

// Called to bind functionality to input
void AActionCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(IA_Test, ETriggerEvent::Started, this, &AActionCharacter::OnTestAction);
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AActionCharacter::OnMoveInput);
		EnhancedInputComponent->BindActionValueLambda(IA_Sprint, ETriggerEvent::Started,
			[this](const FInputActionValue& _) {	// 형식 맞추느라 변수 넣지만, 쓰진 않을 때 _ 이름 사용
				OnSprintStart();
			});
		EnhancedInputComponent->BindActionValueLambda(IA_Sprint, ETriggerEvent::Completed,
			[this](const FInputActionValue& _) {	// 형식 맞추느라 변수 넣지만, 쓰진 않을 때 _ 이름 사용
				OnSprintEnd();
			});
		EnhancedInputComponent->BindAction(IA_Roll, ETriggerEvent::Started, this, &AActionCharacter::OnRollAction);
	}

}

void AActionCharacter::OnTestAction(const FInputActionValue& Value)
{
	// Value.Get<bool>();
	// Value.Get<FVector2D>();

	UE_LOG(LogTemp, Log, TEXT("TestAction 실행"));
}

void AActionCharacter::OnMoveInput(const FInputActionValue& Value)
{
	FVector2D InputValue = Value.Get<FVector2D>();
	FVector WorldDirection = FVector(InputValue.Y, InputValue.X, 0).GetSafeNormal();

	//UE_LOG(LogTemp, Log, TEXT("Input: %.1f, %.1f"), InputValue.X, InputValue.Y);
	//UE_LOG(LogTemp, Log, TEXT("WorldDirectioin : %.1f, %.1f"), WorldDirection.X, WorldDirection.Y);
	//UE_LOG(LogTemp, Log, InputValue.ToString());

	// 카메라의 Yaw 회전각(Degree)을 Radian으로 변경
	float YawRadian = FMath::DegreesToRadians(GetControlRotation().Yaw);
	
	// 좌우 회전만 할 것이기 때문에 UpVector를 기준축으로 Yaw 회전각만큼 돌리는 회전 만들기
	FQuat ControlYawRotation(FVector::UpVector, YawRadian);
	
	// 입력된 방향에 회전 적용 (= 카메라 Yaw 회전만큼 입력 방향을 회전시키기)
	WorldDirection = ControlYawRotation.RotateVector(WorldDirection);

	AddMovementInput(WorldDirection);

}

void AActionCharacter::OnSprintStart()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	bSprintMode = true;

}

void AActionCharacter::OnSprintEnd()
{
	GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	bSprintMode = false;

}

void AActionCharacter::OnRollAction(const FInputActionValue& Value)
{
	if (!RollMontage)
		return;

	if (!AnimInstance)
	{
		AnimInstance = GetMesh()->GetAnimInstance();
	}

	// 스태미너 소비 시도 후 소비되면 구르기 실행
	if (IStaminaInterface::Execute_ConsumeStamina(StatComponent, RollStaminaCost))
	{
		if (AnimInstance && !AnimInstance->IsAnyMontagePlaying())
		{
			if (!GetLastMovementInputVector().IsNearlyZero())	// 이동 입력 중이면
			{
				SetActorRotation(GetLastMovementInputVector().Rotation());	// 입력 방향으로 즉시 회전해서 구르기
			}

			PlayAnimMontage(RollMontage.Get());

		}
	}
}
