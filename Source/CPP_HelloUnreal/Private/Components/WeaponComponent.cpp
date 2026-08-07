// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/WeaponComponent.h"
#include "CPP_HelloUnreal/CPP_HelloUnreal.h"
#include "Interface/WEaponUserInterface.h"
#include "Weapons/WeaponActor.h"
#include "Datas/Items/WeaponDataAsset.h"
#include "AnimNotify/AnimNotifyState_SectionJump.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OwnerCharacter = Cast<ACharacter>(GetOwner());

	if (OwnerCharacter.IsValid())
	{
		// 애님 인스턴스 캐싱
		if (OwnerCharacter->GetMesh())
		{
			OwnerAnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
		}

		// 기본 무기 장착
		if (DefaultWeaponData)
		{
			IWeaponUserInterface::Execute_EquipWeapon(OwnerCharacter.Get(), DefaultWeaponData);
		}
	}
}

// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWeaponComponent::SpawnWeaponActorAndEquip()
{
	if (!CurrentWeaponData || !OwnerCharacter.IsValid())
	{
		return;	// 로딩 요청이 끝나기 전에 해제되었을 때를 대비
	}
	UE_LOG(LogTemp, Log, TEXT("무기 만들쟈이"));
	CurrentWeapon = GetWorld()->SpawnActorDeferred<AWeaponActor>(
		AWeaponActor::StaticClass(), FTransform::Identity, OwnerCharacter.Get(), OwnerCharacter.Get());	// 스폰 시작

	if (CurrentWeapon.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("무기 만들었다이"));
		CurrentWeapon->InitializeWeapon(CurrentWeaponData);
		CurrentWeapon->OnWeaponDrop.BindUObject(this, &UWeaponComponent::OnWeaponDrop);
		UGameplayStatics::FinishSpawningActor(CurrentWeapon.Get(), FTransform::Identity);	// 스폰 완료 (= BeginPlay까지 실행)

		CurrentWeapon->EquipToTarget(OwnerCharacter.Get());
	}
}

void UWeaponComponent::EquipWeapon(UWeaponDataAsset* InWeaponData)
{
	if (InWeaponData == CurrentWeaponData)
	{
		// 같은 종류의 무기 획득: 내구도 초기화
		CurrentWeapon->ResetUseCount();
	}
	else
	{
		// 다른 종류의 무기 장비
		
		// 이전 무기 해제
		if (CurrentWeapon.IsValid())
		{
			CurrentWeapon.Get()->DropWeapon();
			CurrentWeapon = nullptr;
		}

		// 새 무기 장비
		CurrentWeaponData = InWeaponData;
		if (CurrentWeaponData)	// null일때는 장비 안 함
		{
			if (!CurrentWeaponData->IsLoaded())
			{
				// 데이터가 로딩 안 되어 있으면 로딩 요청
				UWeaponDataAsset* RequestedData = CurrentWeaponData;
				CurrentWeaponData->RequestDataLoad(
					FStreamableDelegate::CreateWeakLambda(
						this,
						[this, RequestedData]()
						{
							// 로딩이 완료되면 실행되는 람다 함수
							if (CurrentWeaponData == RequestedData)
							{
								// 중복으로 로딩 요청했을 때를 대비
								SpawnWeaponActorAndEquip();
							}
						}
					)
				);
			}
			else
			{
				// 무기 정보가 로딩된 상황이면 즉시 스폰하고 장비
				SpawnWeaponActorAndEquip();
			}
		}
	}
}

void UWeaponComponent::OnWeaponDrop(UWeaponDataAsset* InDropWeaponData)
{
	if (DefaultWeaponData && (DefaultWeaponData != InDropWeaponData))
	{
		EquipWeapon(DefaultWeaponData);
	}
}

void UWeaponComponent::OnWeaponAttackState(bool bEnable)
{
	//OnWeaponAttackStateChanged.Execute(bEnable);		// 밑에랑 똑같은 거. 맨손일 때 터질 위험 있어서 아래꺼 사용.
	OnWeaponAttackStateChanged.ExecuteIfBound(bEnable);	// 바인딩 되어 있으면 실행
}

bool UWeaponComponent::CanWeaponUse()
{
	return CurrentWeapon.IsValid() && CurrentWeapon->CanUse();
}

void UWeaponComponent::SetSectionJumpNotify(UAnimNotifyState_SectionJump* InSectionJumpNotify)
{
	SectionJumpNotify = InSectionJumpNotify;
	bComboReady = SectionJumpNotify.IsValid();
}

bool UWeaponComponent::Attack()
{
	if (!OwnerCharacter.IsValid() || !OwnerAnimInstance.IsValid())
		return false;

	bool bResult = false;

	if (!OwnerAnimInstance->IsAnyMontagePlaying())
	{
		// 첫번째 콤보 공격
		OwnerCharacter->PlayAnimMontage(CurrentWeaponData->AttackMontage);

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &UWeaponComponent::OnAttackEnded);
		OwnerAnimInstance->Montage_SetEndDelegate(EndDelegate, CurrentWeaponData->AttackMontage);

		OnWeaponAttackState(false);

		bResult = true;
	}
	else if (OwnerAnimInstance->GetCurrentActiveMontage() == CurrentWeaponData->AttackMontage)
	{
		//UE_LOG(LogTemp, Log, TEXT("다음 섹션으로 넘어가쇼"));
		bResult = SectionJumpForCombo();
	}

	return bResult;
}

void UWeaponComponent::AreaAttack()
{
	if (!CurrentWeaponData || !CurrentWeaponData || !OwnerCharacter.IsValid())
		return;

	// 디버그 정보 출력
	DrawDebugSphere(
		GetWorld(),
		CurrentWeapon->GetWeaponImpactLocation(),
		CurrentWeaponData->AreaAttackInnerRadius,
		12,
		FColor::Red,
		false,
		5.f
	);
	DrawDebugSphere(
		GetWorld(),
		CurrentWeapon->GetWeaponImpactLocation(),
		CurrentWeaponData->AreaAttackOuterRadius,
		12,
		FColor::Yellow,
		false,
		5.f
	);

	TArray<AActor*> IgnoreActors = { CurrentWeapon.Get(), OwnerCharacter.Get() };

	UGameplayStatics::ApplyRadialDamageWithFalloff(
		GetWorld(),
		CurrentWeaponData->AreaAttackPower,
		1,
		CurrentWeapon->GetWeaponImpactLocation(),
		CurrentWeaponData->AreaAttackInnerRadius,
		CurrentWeaponData->AreaAttackOuterRadius,
		1.f,	// 1일 때(거리에 정비례해서 감소), 0에 가까워질 때(
		nullptr,
		IgnoreActors,
		CurrentWeapon.Get(),
		OwnerCharacter.Get()->GetController(),
		ECC_Enemy
	);
}

bool UWeaponComponent::SectionJumpForCombo()
{
	if (!OwnerAnimInstance.IsValid())
		return false;

	bool bResult = false;

	if (SectionJumpNotify.IsValid() && bComboReady)
	{
		OnAttackEnded(nullptr, true);	// 콤보로 몽타주가 시작되었다 => 이전 애니메이션이 끝났다 => 횟수 감소

		//UE_LOG(LogTemp, Log, TEXT("섹션 넘어간다잇"));
		UAnimMontage* CurrentMontage = OwnerAnimInstance->GetCurrentActiveMontage();
		//AnimInstance->Montage_SetNextSection(	// 섹션을 변경한다.
		//AnimInstance->Montage_GetCurrentSection(CurrentMontage),	// 이 섹션에서 (from)
		//	SectionJumpNotify->GetNextSectionName(),				// 이 섹션으로 변경 (to)
		//	CurrentMontage	// 적용할 몽타주
		//);
		OwnerAnimInstance->Montage_JumpToSection(	// 섹션을 변경한다.
			SectionJumpNotify->GetNextSectionName(),				// 이 섹션으로 변경 (to)
			CurrentMontage	// 적용할 몽타주
		);
		//UE_LOG(LogTemp, Log, TEXT("섹션 넘어갔다리"));

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &UWeaponComponent::OnAttackEnded);
		OwnerAnimInstance->Montage_SetEndDelegate(EndDelegate, CurrentWeaponData->AttackMontage);	// 애니메이션이 끝나는 타이밍에 실행

		OnWeaponAttackState(false);
		//IStaminaInterface::Execute_ConsumeStamina(GetStatComponent(), AttackCost);
		bComboReady = false;	// 중복 실행 방지

		bResult = true;	// 공격 성공
	}

	return bResult;
}

void UWeaponComponent::OnAttackEnded(UAnimMontage* InMontage, bool bInterrupted)
{
	UE_LOG(LogTemp, Log, TEXT("OnAttackEnded"));
	if (CurrentWeapon.IsValid())
	{
		CurrentWeapon->Use();
	}
}
