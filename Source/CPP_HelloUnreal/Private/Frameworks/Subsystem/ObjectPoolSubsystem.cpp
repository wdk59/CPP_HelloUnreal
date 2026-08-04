// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/Subsystem/ObjectPoolSubsystem.h"
#include "Config/ObjectPoolSettings.h"

void UObjectPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 프로젝트 세팅에서 데이터 읽어오기
	const UObjectPoolSettings* Settings = GetDefault<UObjectPoolSettings>();
	if (Settings)
	{
		if (!Settings->DamagePopupClass)
		{
			// LoadSynchronous: 즉시 로딩
			DamagePopupClass = Settings->DamagePopupClass.LoadSynchronous();
		}
	}
}

AActor* UObjectPoolSubsystem::Spawn(const FTransform& InTransform)
{
	AActor* Spawned = nullptr;

	if (ReadyActors.Num() > 0)
	{
		ReadyActors.Pop();	// 뒤에서 꺼내기
	}
	else
	{
		if (DamagePopupClass && GetWorld())
		{
			FActorSpawnParameters SpawnParam;
			SpawnParam.Owner = nullptr;
			SpawnParam.ObjectFlags = RF_Transient;

			Spawned = GetWorld()->SpawnActor<AActor>(DamagePopupClass, InTransform, SpawnParam);

#if WITH_EDITOR
			Spawned->SetFolderPath(FName("Pool"));	// 아웃라이너에 표시될 폴더 정리
#endif
		}
	}

	if (Spawned)
	{
		ActiveActors.Add(Spawned);
	}

	return Spawned;
}

void UObjectPoolSubsystem::ReturnPool(AActor* InActor)
{
	if (!InActor)
		return;

	ActiveActors.Remove(InActor);
	ReadyActors.Add(InActor);
}
