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

// Called when the game starts or when spawned
void AActionCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AActionCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AActionCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(IA_Test, ETriggerEvent::Started, this, &AActionCharacter::OnTestAction);
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AActionCharacter::OnMoveInput);
		EnhancedInputComponent->BindAction(IA_Sprint, ETriggerEvent::Started, this, &AActionCharacter::OnSprintAction);
		EnhancedInputComponent->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &AActionCharacter::OnStopSprintAction);
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

	if (Controller != nullptr)
	{
		// Get Yaw Axis from Controller Rotation
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		
		// Get Yaw Unit Vector based on Ground
		const FVector Forward = UKismetMathLibrary::GetForwardVector(YawRotation);
		const FVector Right = UKismetMathLibrary::GetRightVector(YawRotation);

		// Apply Input Movement
		AddMovementInput(Forward, InputValue.Y);
		AddMovementInput(Right, InputValue.X);
	}

}

void AActionCharacter::OnSprintAction()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AActionCharacter::OnStopSprintAction()
{
	GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
}

