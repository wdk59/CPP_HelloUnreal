// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ActionCharacter.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StatComponent.h"
#include "Animation/AnimMontage.h"
#include "AnimNotify/AnimNotifyState_SectionJump.h"
#include "Datas/WeaponDataAsset.h"
#include "Weapons/WeaponActor.h"
#include "Kismet/GameplayStatics.h"

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

UStatComponent* AActionCharacter::GetStatComponent() const
{
	return StatComponent;
}

void AActionCharacter::OnWeaponAttackState(bool bEnable)
{
	//OnOnWeaponAttackStateChanged.Execute(bEnable);		// 밑에랑 똑같은 거. 맨손일 때 터질 위험 있어서 아래꺼 사용.
	OnOnWeaponAttackStateChanged.ExecuteIfBound(bEnable);	// 바인딩 되어 있으면 실행
}

void AActionCharacter::SetSectionJumpNotify(UAnimNotifyState_SectionJump* InSectionJumpNotify)
{
	SectionJumpNotify = InSectionJumpNotify;
	bComboReady = SectionJumpNotify.IsValid();
	/*if (SectionJumpNotify.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("점프 가능"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("늦었슈"));
	}*/
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

	CurrentWeapon = nullptr;

	IWeaponUserInterface::Execute_EquipWeapon(this, DefaultWeaponData);

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

void AActionCharacter::SectionJumpForCombo()
{
	if (SectionJumpNotify.IsValid() && bComboReady)
	{
		//UE_LOG(LogTemp, Log, TEXT("섹션 넘어간다잇"));
		UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
		//AnimInstance->Montage_SetNextSection(	// 섹션을 변경한다.
		//AnimInstance->Montage_GetCurrentSection(CurrentMontage),	// 이 섹션에서 (from)
		//	SectionJumpNotify->GetNextSectionName(),				// 이 섹션으로 변경 (to)
		//	CurrentMontage	// 적용할 몽타주
		//);
		AnimInstance->Montage_JumpToSection(	// 섹션을 변경한다.
			SectionJumpNotify->GetNextSectionName(),				// 이 섹션으로 변경 (to)
			CurrentMontage	// 적용할 몽타주
		);
		//UE_LOG(LogTemp, Log, TEXT("섹션 넘어갔다리"));

		OnWeaponAttackState(false);
		IStaminaInterface::Execute_ConsumeStamina(GetStatComponent(), AttackCost);
		bComboReady = false;	// 중복 실행 방지
	}
}

void AActionCharacter::SpawnWeaponActor()
{
	if (!CurrentWeaponData)
	{
		return;	// 로딩 요청이 끝나기 전에 해제되었을 때를 대비
	}
	UE_LOG(LogTemp, Log, TEXT("무기 만들쟈이"));
	CurrentWeapon = GetWorld()->SpawnActorDeferred<AWeaponActor>(
		AWeaponActor::StaticClass(), FTransform::Identity, this, this);	// 스폰 시작

	if (CurrentWeapon.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("무기 만들었다이"));
		CurrentWeapon->InitializeWeapon(CurrentWeaponData);
		UGameplayStatics::FinishSpawningActor(CurrentWeapon.Get(), FTransform::Identity);	// 스폰 완료 (= BeginPlay까지 실행)
	
		CurrentWeapon->EquipToTarget(this);

		if (CurrentWeaponData == DefaultWeaponData)
		{
			DefaultWeapon = CurrentWeapon.Get();
			DefaultWeapon->SetToDefaultWeapon();
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
		EnhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Started, this, &AActionCharacter::OnAttackAction);
	}

}

void AActionCharacter::LoadWeaponData(UWeaponDataAsset* RequestedData)
{
	if (!IsValid(RequestedData))
	{
		UE_LOG(LogTemp, Error, TEXT("failed LoadWeaponData: Invalid RequestedData"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("무기 데이터 로드!"));
	if (!RequestedData->IsLoaded())
	{
		UE_LOG(LogTemp, Log, TEXT("새로 로드!"));
		RequestedData->RequestDataLoad(
			FStreamableDelegate::CreateWeakLambda(this, [this, RequestedData]()
				{
					// 로딩이 완료되면 실행되는 람다 함수
					if (CurrentWeaponData == RequestedData)
					{
						UE_LOG(LogTemp, Log, TEXT("로드 끝, 스폰!"));
						SpawnWeaponActor();
					}
				})
		);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("바로 스폰!"));
		SpawnWeaponActor();
	}
}

void AActionCharacter::EquipWeapon_Implementation(UWeaponDataAsset* InWeaponData)
{
	if (!IsValid(InWeaponData))
	{
		UE_LOG(LogTemp, Error, TEXT("failed EqupWeapon: Invalid InWeaponData"));
		return;
	}

	// 이전 무기 해제하기
	IWeaponUserInterface::Execute_UnequipWeapon(this);

	// 새 무기 장착하기
	CurrentWeaponData = InWeaponData;
	LoadWeaponData(InWeaponData);
}

void AActionCharacter::UnequipWeapon_Implementation()
{
	if (CurrentWeapon.IsValid())
	{
		if (CurrentWeapon == DefaultWeapon)
		{
			// 기본 무기 비활성화
			DeactiveDefaultWeapon();
		}
		else
		{
			// 이전 무기 Destroy
			CurrentWeapon->DropWeapon();
			CurrentWeapon = nullptr;
			CurrentWeaponData = DefaultWeaponData;
		}
	}
}

void AActionCharacter::EquipDefaultWeapon_Implementation()
{
	if (CurrentWeaponData && CurrentWeaponData != DefaultWeaponData)
	{
		// 이전 무기 해제하기
		IWeaponUserInterface::Execute_UnequipWeapon(this);
	}

	// 기본 무기 활성화
	ActiveDefaultWeapon();
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

void AActionCharacter::OnAttackAction(const FInputActionValue& Value)
{

	if (AnimInstance)
	{
		if (!AnimInstance->IsAnyMontagePlaying())
		{
			// 첫번째 콤보 공격
			PlayAnimMontage(AttackMontage);
			OnWeaponAttackState(false);
			IStaminaInterface::Execute_ConsumeStamina(GetStatComponent(), AttackCost);
		}
		else if (AnimInstance->GetCurrentActiveMontage() == AttackMontage)
		{
			//UE_LOG(LogTemp, Log, TEXT("다음 섹션으로 넘어가쇼"));
			SectionJumpForCombo();
		}
	}
}

float AActionCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (UStatComponent* StatComp = GetStatComponent())
	{
		IHealthInterface::Execute_ReceiveDamage(StatComp, Damage);
		UE_LOG(LogTemp, Log, TEXT("아야 (데미지: %.1f, 공격자: %s)"), Damage, *EventInstigator->GetName());

		// 내 코드
		/*if (StatComp->Implements<UHealthInterface>())
		{

			if (DamageAmount > 0)
			{
				IHealthInterface::Execute_ReceiveDamage(StatComp, DamageAmount);
			}
			else
			{
				IHealthInterface::Execute_RecoveryHealth(StatComp, DamageAmount);
			}
		}*/
	}

	return Damage;
}

void AActionCharacter::ActiveDefaultWeapon()
{
	if (!IsValid(DefaultWeapon))
	{
		UE_LOG(LogTemp, Error, TEXT("failed ActiveDefaultWeapon: Invalid DefaultWeapon"));
		CurrentWeapon = nullptr;
		return;
	}

	CurrentWeapon = DefaultWeapon;
	CurrentWeapon->SetActorHiddenInGame(false);
	CurrentWeapon->SetActorEnableCollision(true);
	CurrentWeapon->SetActorTickEnabled(true);
}

void AActionCharacter::DeactiveDefaultWeapon()
{
	if (!IsValid(DefaultWeapon))
	{
		UE_LOG(LogTemp, Error, TEXT("failed ActiveDefaultWeapon: Invalid DefaultWeapon"));
		CurrentWeapon = nullptr;
		return;
	}

	CurrentWeapon->SetActorHiddenInGame(true);
	CurrentWeapon->SetActorEnableCollision(false);
	CurrentWeapon->SetActorTickEnabled(false);
	CurrentWeapon = nullptr;
}
