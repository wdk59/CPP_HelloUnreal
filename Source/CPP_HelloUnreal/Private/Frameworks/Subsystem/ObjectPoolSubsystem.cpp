// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/Subsystem/ObjectPoolSubsystem.h"
#include "Config/ObjectPoolSettings.h"
#include "Interface/PoolableInterface.h"

void UObjectPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogTemp, Log, TEXT("ObjectPool Subsystem Initialize"));

	// 프로젝트 세팅에서 데이터 읽어오기
	const UObjectPoolSettings* Settings = GetDefault<UObjectPoolSettings>();
	if (Settings)
	{
		if (!Settings->DamagePopupClass.IsNull())
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
		Spawned = ReadyActors.Pop();	// 뒤에서 꺼내기
		Spawned->SetActorTransform(InTransform);
		UE_LOG(LogTemp, Log, TEXT("Spawn(Reuse) : %s"), Spawned  ? *Spawned->GetName() : TEXT("None"));
	}
	else
	{
		if (DamagePopupClass && GetWorld())
		{
			FActorSpawnParameters SpawnParam;
			SpawnParam.Owner = nullptr;
			SpawnParam.ObjectFlags = RF_Transient;

			Spawned = GetWorld()->SpawnActor<AActor>(DamagePopupClass, InTransform, SpawnParam);
			UE_LOG(LogTemp, Log, TEXT("Spawn(New) : %s"), Spawned  ? *Spawned->GetName() : TEXT("None"));

#if WITH_EDITOR
			Spawned->SetFolderPath(FName("Pool"));	// 아웃라이너에 표시될 폴더 정리
#endif
		}
	}

	if (Spawned)
	{
		if (Spawned->GetClass()->ImplementsInterface(UPoolableInterface::StaticClass()))
		{
			//UE_LOG(LogTemp, Log, TEXT("OnSpawn"));
			IPoolableInterface::Execute_OnSpawn(Spawned);
		}

		ActiveActors.Add(Spawned);
	}

	return Spawned;
}

void UObjectPoolSubsystem::ReturnPool(AActor* InActor)
{
	if (!InActor)
		return;

	if (!ActiveActors.Contains(InActor))
		return;

	UE_LOG(LogTemp, Log, TEXT("Return : %s"), InActor ? *InActor->GetName() : TEXT("None"));

	if (InActor->GetClass()->ImplementsInterface(UPoolableInterface::StaticClass()))
	{
		IPoolableInterface::Execute_OnReturn(InActor);
	}

	ActiveActors.Remove(InActor);
	ReadyActors.Add(InActor);
}
