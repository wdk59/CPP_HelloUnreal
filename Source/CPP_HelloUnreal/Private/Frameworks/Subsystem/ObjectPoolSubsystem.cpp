// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/Subsystem/ObjectPoolSubsystem.h"
#include "Config/ObjectPoolSettings.h"
#include "Interface/PoolableInterface.h"
#include "Datas/ObjectPoolDataAsset.h"

void UObjectPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogTemp, Log, TEXT("ObjectPool Subsystem Initialize"));

	// 프로젝트 세팅에서 데이터 읽어오기
	const UObjectPoolSettings* Settings = GetDefault<UObjectPoolSettings>();
	if (!Settings)
		return;

	for (const TSoftObjectPtr<UObjectPoolDataAsset>& DataAsset : Settings->PoolDataAssets)
	{
		if (!DataAsset.IsNull())
		{
			TObjectPtr<UObjectPoolDataAsset> LoadedDataAsset = DataAsset.LoadSynchronous();
			FObjectPool& Pool = ObjectPools.FindOrAdd(LoadedDataAsset->ActorClass.LoadSynchronous());
			Pool.InitialSize = DataAsset->InitialSize;
		}
	}
}

void UObjectPoolSubsystem::Deinitialize()
{
	//ClearAllPools();

	Super::Deinitialize();
}

bool UObjectPoolSubsystem::RegisterPoolDataAsset(const UObjectPoolDataAsset* InDataAsset, bool bWarmup)
{
	if (!InDataAsset || InDataAsset->ActorClass.IsNull())
		return false;

	// 이미 로드된 상황이면 바로 리턴이 되기 때문에, 성능은 걱정하지 않아도 괜찮음
	TSubclassOf<AActor> LoadedActorClass = InDataAsset->ActorClass.LoadSynchronous();
	
	ClearPool(LoadedActorClass);	// 이미 있으면 정리

	FObjectPool& Pool = ObjectPools.Add(LoadedActorClass);	// 새로 추가
	Pool.InitialSize = InDataAsset->InitialSize;			// 초기값 세팅

	if (bWarmup)
	{
		Warmup(LoadedActorClass);	// 웜업 요청 있으면 웜업도 수행
	}

	return true;
}

bool UObjectPoolSubsystem::UnRegisterPoolDataAsset(const UObjectPoolDataAsset* InDataAsset)
{
	if (!InDataAsset || InDataAsset->ActorClass.IsNull())
		return false;

	TSubclassOf<AActor> LoadedActorClass = InDataAsset->ActorClass.LoadSynchronous();
	ClearPool(LoadedActorClass);

	return true;
}

void UObjectPoolSubsystem::Warmup(TSubclassOf<AActor> InClass)
{
	if (FObjectPool* Pool = ObjectPools.Find(InClass))
	{
		FTransform Init(FVector::DownVector * 10000.f);
		
		TArray<TWeakObjectPtr<AActor>> SpawnedArray;
		SpawnedArray.Reserve(Pool->InitialSize);
		for (int i = 0; i < Pool->InitialSize; i++)
		{
			SpawnedArray.Add(Spawn(InClass, Init));
		}
		
		// 스폰하자마자 풀에 넣으면 스폰 요청했을 때
		// 새로운 액터를 생성하지 않고, 풀에 있는 거를 가져오기 때문에
		// 미리 필요한만큼 전부 생성한 다음에 풀에 넣음
		for (TWeakObjectPtr<AActor> Spawned : SpawnedArray)
		{
			ReturnPool(Spawned.Get());
		}
	}
}

void UObjectPoolSubsystem::WarmupAll()
{
	for (auto& [Key, _] : ObjectPools)
	{
		Warmup(Key);
	}
}

void UObjectPoolSubsystem::ClearPool(TSubclassOf<AActor> InClass)
{
	if (FObjectPool* Pool = ObjectPools.Find(InClass))
	{
		for (AActor* Actor : Pool->ReadyActors)
		{
			if (IsValid(Actor))
				Actor->Destroy();
		}
		Pool->ReadyActors.Empty();

		for (AActor* Actor : Pool->ActiveActors)
		{
			if (IsValid(Actor))
				Actor->Destroy();
		}
		Pool->ActiveActors.Empty();

		ObjectPools.Remove(InClass);
	}
}

void UObjectPoolSubsystem::ClearAllPools()
{
	for (auto& [Key, _] : ObjectPools)
	{
		ClearPool(Key);
	}
	ObjectPools.Empty();
}

AActor* UObjectPoolSubsystem::Spawn(const TSubclassOf<AActor> InClassType, const FTransform& InTransform)
{
	if (!InClassType)
		return nullptr;

	FObjectPool* Pool = ObjectPools.Find(InClassType);
	if (!Pool)
		return nullptr;

	AActor* Spawned = nullptr;

	if (Pool->ReadyActors.Num() > 0)
	{
		Spawned = Pool->ReadyActors.Pop();	// 뒤에서 꺼내기
		Spawned->SetActorTransform(InTransform);
		UE_LOG(LogTemp, Log, TEXT("Spawn(Reuse) : %s"), Spawned  ? *Spawned->GetName() : TEXT("None"));
	}
	else
	{
		if (GetWorld())
		{
			FActorSpawnParameters SpawnParam;
			SpawnParam.Owner = nullptr;
			SpawnParam.ObjectFlags = RF_Transient;

			Spawned = GetWorld()->SpawnActor<AActor>(InClassType, InTransform, SpawnParam);
			UE_LOG(LogTemp, Log, TEXT("Spawn(New) : %s"), Spawned  ? *Spawned->GetName() : TEXT("None"));

#if WITH_EDITOR
			if (Spawned)
			{
				Spawned->SetFolderPath(FName("Pool"));	// 아웃라이너에 표시될 폴더 정리
			}
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
		else
		{
			Spawned->SetActorHiddenInGame(false);
			Spawned->SetActorTickEnabled(true);
			Spawned->SetActorEnableCollision(true);
		}

		Pool->ActiveActors.Add(Spawned);
	}

	return Spawned;
}

void UObjectPoolSubsystem::ReturnPool(AActor* InActor)
{
	if (!InActor)
		return;

	TSubclassOf<AActor> InActorClass = InActor->GetClass();
	FObjectPool* Pool = ObjectPools.Find(InActorClass);

	if (!Pool || !Pool->ActiveActors.Contains(InActor))
		return;

	UE_LOG(LogTemp, Log, TEXT("Return : %s"), InActor ? *InActor->GetName() : TEXT("None"));

	if (InActor->GetClass()->ImplementsInterface(UPoolableInterface::StaticClass()))
	{
		IPoolableInterface::Execute_OnReturn(InActor);
	}
	else
	{
		InActor->SetActorHiddenInGame(true);
		InActor->SetActorTickEnabled(false);
		InActor->SetActorEnableCollision(false);
	}

	Pool->ActiveActors.Remove(InActor);
	Pool->ReadyActors.Add(InActor);
}
