// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponActor.generated.h"

class UCapsuleComponent;

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

protected :

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCapsuleComponent> HitArea = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName AttachSocketName = TEXT("SwordSocket");

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackDamage = 10.f;

private :

	// 무기를 장비하고 있는 대상
	TWeakObjectPtr<ACharacter> OwnerCharacter = nullptr;

public :

	UFUNCTION(BlueprintCallable)
	void AttackEnable(bool bEnable);

protected :

	UFUNCTION(BlueprintCallable)
	void OnEquipped(AActor* InOwner);

	UFUNCTION()
	void OnHitAreaBeginOverlap(
		UPrimitiveComponent*	InOverlapComponent,
		AActor*					InOtherActor,
		UPrimitiveComponent*	InOthercomp,
		int32					InOtherBodyIndex,
		bool					bFromSweep,	// 스윕으로 일어나냐 아니냐
		const FHitResult&		InSweepResult);

};
