// Fill out your copyright notice in the Description page of Project Settings.


#include "Test02/ActionAnimInstance.h"
#include "GameFramework/PawnMovementComponent.h"

void UActionAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();


	if (APawn* OwnerPawn = TryGetPawnOwner())
	{
		OwnerMovementComponent = OwnerPawn->GetMovementComponent();	// Owner 찾는 건 한 번만
	}
}

void UActionAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (OwnerMovementComponent.IsValid())
	{
		MoveSpeed = OwnerMovementComponent->Velocity.Size();
	}
	else
	{
		// 처음에 못 찾았을 대를 대비한 코드
		if (APawn* OwnerPawn = TryGetPawnOwner())
		{
			OwnerMovementComponent = OwnerPawn->GetMovementComponent();
		}
	}
}
