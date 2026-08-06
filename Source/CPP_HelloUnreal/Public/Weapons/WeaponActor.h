// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponActor.generated.h"

DECLARE_DELEGATE_OneParam(FOnWeaponDrop, UWeaponDataAsset*);

class ACharacter;
class UCapsuleComponent;
class UNiagaraComponent;

UCLASS()
class CPP_HELLOUNREAL_API AWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHitAreaBeginOverlap(
		UPrimitiveComponent* InOverlapComponent,
		AActor* InOtherActor,
		UPrimitiveComponent* InOthercomp,
		int32					InOtherBodyIndex,
		bool					bFromSweep,	// 스윕으로 일어나냐 아니냐
		const FHitResult& InSweepResult
	);


public :

	FOnWeaponDrop OnWeaponDrop;

protected :

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCapsuleComponent> HitArea = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> Mesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> TrailVFX = nullptr;

	// 무기 데이터 에셋
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UWeaponDataAsset> WeaponData;

	// 무기 사용 횟수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponData")
	int32 CurrentUseCount = 1;

	// 무기가 드랍된 후 사라질 때까지의 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DropLifeSpawn = 10.f;

	// 드랍 직후에 플레이어와 물리 상호작용이 안 되는 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PhysicsDelay = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ThrowPower = 700.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DropLifeSpan = 10.f;

private :

	// 무기를 장비하고 있는 대상
	TWeakObjectPtr<ACharacter> OwnerCharacter = nullptr;

	// PhysicsDelay용 타이머 핸들
	FTimerHandle PhysicsDelayTimerHandle;

public :

	UFUNCTION(BlueprintCallable)
	void InitializeWeapon(UWeaponDataAsset* InData);

	UFUNCTION(BlueprintCallable)
	void EquipToTarget(AActor* Target);

	UFUNCTION(BlueprintCallable)
	void DropWeapon();

	UFUNCTION(BlueprintCallable)
	bool CanUse() const { return CurrentUseCount > 0; }

	UFUNCTION(BlueprintCallable)
	void Use();

	UFUNCTION(BlueprintCallable)
	void ResetUseCount();

	UFUNCTION(BlueprintCallable)
	FVector GetWeaponImpactLocation() const;

protected :

	UFUNCTION(BlueprintCallable)
	void OnEquipped(AActor* InOwner);

	UFUNCTION(BlueprintCallable)
	void AttackEnable(bool bEnable);

};
