// Fill out your copyright notice in the Description page of Project Settings.


#include "Test02/ActionCharacter.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AActionCharacter::AActionCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArmComponent->SetupAttachment(RootComponent);
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(CameraSpringArmComponent);
	
	bUseControllerRotationYaw = false;	// 컨트롤러 움직일 때 폰이 같이 회전되는 것 방지
	GetCharacterMovement()->bOrientRotationToMovement = true;	// 캐릭터 이동 방향으로 바라보게 만들기
	CameraSpringArmComponent->bUsePawnControlRotation = true;	// 스프링암은 컨트롤러 입력대로 회전

}

float AActionCharacter::GetCurrentStamina_Implementation() const
{
	return CurrentStamina;
}

bool AActionCharacter::ConsumeStamina_Implementation(float InAmount)
{
	bool bResult = false;

	if (CurrentStamina >= InAmount)
	{
		if (bIsStaminaRecoveryIntervalWorking || bIsRecovering)
		{
			bIsRecovering = false;
			bIsStaminaRecoveryIntervalWorking = false;
			StaminaRecoveryInterval = 0.f;
		}

		CurrentStamina -= InAmount;
		bResult = true;
	}

	UE_LOG(LogTemp, Log, TEXT("현재 Stamina: %.1f"), CurrentStamina);

	return bResult;
}

void AActionCharacter::RecoveryStamina_Implementation(float InAmount)
{
	CurrentStamina = FMath::Clamp(CurrentStamina + InAmount, 0, MaxStamina);
	
	if (CurrentStamina >= MaxStamina)
	{
		bIsRecovering = false;
	}

	UE_LOG(LogTemp, Log, TEXT("현재 Stamina: %.1f"), CurrentStamina);

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

	bIsSprinting = false;
	bSprintSucceed = false;

	CurrentStamina = MaxStamina;

	//GetCurrentStamina();	// 실행했을 때 C++에 구현된 내용만 호출
	//IStaminaInterface::Execute_GetCurrentStamina(this);	// 실행했을 때 블루프린트 구현으로 호출

	StaminaRecoveryInterval = 0.f;
	bIsStaminaRecoveryIntervalWorking = false;
	bIsRecovering = false;

}

// Called every frame
void AActionCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (StaminaRecoveryInterval > 0)
	{
		if (!bIsStaminaRecoveryIntervalWorking)
			bIsStaminaRecoveryIntervalWorking = true;
		
		StaminaRecoveryInterval -= DeltaTime;
	}
	else if (bIsStaminaRecoveryIntervalWorking)
	{
		bIsStaminaRecoveryIntervalWorking = false;
		
		bIsRecovering = true;
	}
	

	if (bIsRecovering)
	{
		IStaminaInterface::Execute_RecoveryStamina(this, StaminaRecoveryAmountPerSecond * DeltaTime);
	}

	if (bIsSprinting)
	{
		if (!IStaminaInterface::Execute_ConsumeStamina(this, SprintStaminaCostPerSecond * DeltaTime))
		{
			OnSprintEnd();
		}
		else if (!bSprintSucceed)
		{
			bSprintSucceed = true;
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
		// My Code
		/*EnhancedInputComponent->BindAction(IA_Sprint, ETriggerEvent::Started, this, &AActionCharacter::OnSprintAction);
		EnhancedInputComponent->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &AActionCharacter::OnStopSprintAction);*/
		// Teacher's Code
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

	// My Code
	//if (Controller != nullptr)
	//{
	//	// Get Yaw Axis from Controller Rotation
	//	const FRotator Rotation = Controller->GetControlRotation();
	//	const FRotator YawRotation(0, Rotation.Yaw, 0);
	//	
	//	// Get Yaw Unit Vector based on Ground
	//	const FVector Forward = UKismetMathLibrary::GetForwardVector(YawRotation);
	//	const FVector Right = UKismetMathLibrary::GetRightVector(YawRotation);

	//	// Apply Input Movement
	//	AddMovementInput(Forward, InputValue.Y);
	//	AddMovementInput(Right, InputValue.X);
	//}
	// Teacher's Code
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

// My Code
//void AActionCharacter::OnSprintAction()
//{
//	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
//}
//
//void AActionCharacter::OnStopSprintAction()
//{
//	GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
//}
// Teacher's Code
void AActionCharacter::OnSprintStart()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

	bIsSprinting = true;

}

void AActionCharacter::OnSprintEnd()
{
	GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;

	if (bSprintSucceed)
	{
		bSprintSucceed = false;
		StaminaRecoveryInterval = StaminaRecoveryDelay;
	}

	bIsSprinting = false;

}

void AActionCharacter::OnRollAction(const FInputActionValue& Value)
{
	if (!RollMontage.IsValid())
		return;

	if (!AnimInstance)
	{
		AnimInstance = GetMesh()->GetAnimInstance();
	}

	if (AnimInstance && !AnimInstance->IsAnyMontagePlaying() && IStaminaInterface::Execute_ConsumeStamina(this, RollStaminaCost))
	{
		if (!GetLastMovementInputVector().IsNearlyZero())	// 이동 입력 중이면
		{
			SetActorRotation(GetLastMovementInputVector().Rotation());	// 입력 방향으로 즉시 회전해서 구르기
		}

		PlayAnimMontage(RollMontage.Get());

		StaminaRecoveryInterval = StaminaRecoveryDelay;

	}
}