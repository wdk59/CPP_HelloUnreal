// Fill out your copyright notice in the Description page of Project Settings.


#include "Test02/StaminaPickup.h"
#include "../Interface/StatInterface.h"
#include "../Interface/StaminaInterface.h"
#include "Test02/StatComponent.h"

AStaminaPickup::AStaminaPickup()
{

}

void AStaminaPickup::ChangeStat_Implementation(AActor* OtherActor)
{
	//Super::ChangeStat_Implementation(OtherActor);

	//// Target이 null이 아니면 인터페이스를 상속받았다.(= C++이니까 구현도 되어 있다. 블루프린트에서 상속을 했을 경우에는 체크 불가능)
	////IStaminaInterface* Target = Cast<IStaminaInterface>(OtherActor);
	//// bImplements이 true면 인터페이스를 구현했다.(블루프린트에서의 상속도 체크 가능)
	////bool bImplements = OtherActor->Implements<UStaminaInterface>();

	//if (OtherActor && OtherActor->Implements<UStatInterface>())
	//{
	//	UStatComponent* StatComponent = IStatInterface::GetStatComponent(OtherActor);
	//	if (StatComponent && StatComponent->Implements<UStaminaInterface>())
	//	{
	//		if (Amount > 0)
	//		{
	//			IStaminaInterface::Execute_RecoveryStamina(StatComponent, Amount);
	//		}
	//		else
	//		{
	//			IStaminaInterface::Execute_ConsumeStamina(StatComponent, -Amount);
	//		}
	//	}
	//}
}
