// Fill out your copyright notice in the Description page of Project Settings.


#include "Test02/HealthPickup.h"
#include "../Interface/StatInterface.h"
#include "../Interface/HealthInterface.h"
#include "Test02/StatComponent.h"

AHealthPickup::AHealthPickup()
{

}

void AHealthPickup::ChangeStat_Implementation(AActor* OtherActor)
{
	/*Super::ChangeStat_Implementation(OtherActor);

	if (OtherActor && OtherActor->Implements<UStatInterface>())
	{
		UStatComponent* StatComponent = IStatInterface::Execute_GetStatComponent(OtherActor);

		if (StatComponent && StatComponent->Implements<UHealthInterface>())
		{
			if (Amount > 0)
			{
				IHealthInterface::Execute_RecoveryHealth(StatComponent, Amount);
			}
			else
			{
				IHealthInterface::Execute_ReceiveDamage(StatComponent, -Amount);
			}
		}
	}*/
}
