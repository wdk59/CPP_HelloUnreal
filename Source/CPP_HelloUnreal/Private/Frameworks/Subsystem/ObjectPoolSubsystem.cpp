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
			Pool.MaxSize = LoadedDataAsset->MaxSize;
			Pool.MaxPolicy = LoadedDataAsset->MaxPolicy;
			Pool.InitialSize = LoadedDataAsset->InitialSize;
		}
	}
}

void UObjectPoolSubsystem::Deinitialize()
{
	ClearAllPools();

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
	Pool.MaxSize = InDataAsset->MaxSize;
	Pool.MaxPolicy = InDataAsset->MaxPolicy;

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
	if (!GetWorld())
		return;

	if (FObjectPool* Pool = ObjectPools.Find(InClass))
	{
		FTransform Init(FVector::DownVector * 10000.f);
		
		for (int i = 0; i < Pool->InitialSize; i++)
		{
			// 액터 스폰해서 바로 ReadyActors에 넣기

			/* Spawn */

			AActor* Spawned = CreateNewObject(InClass, Init);
			UE_LOG(LogTemp, Log, TEXT("Warmup : %s"), Spawned ? *Spawned->GetName() : TEXT("None"));

			/* Return */
			if (Spawned) {
				if (Spawned->GetClass()->ImplementsInterface(UPoolableInterface::StaticClass()))
				{
					IPoolableInterface::Execute_OnReturn(Spawned);
				}
				else
				{
					Spawned->SetActorHiddenInGame(true);
					Spawned->SetActorTickEnabled(false);
					Spawned->SetActorEnableCollision(false);
				}

				Pool->ReadyActors.Add(Spawned);
			}
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
		Pool->ActiveOrderList->Empty();
		Pool->ActiveNodeMap->Empty();

		ObjectPools.Remove(InClass);
	}
}

void UObjectPoolSubsystem::ClearAllPools()
{
	for (auto& [Key, Pool] : ObjectPools)
	{
		for (AActor* Actor : Pool.ReadyActors)
		{
			if (IsValid(Actor))
				Actor->Destroy();
		}
		Pool.ReadyActors.Empty();
		
		for (AActor* Actor : Pool.ActiveActors)
		{
			if (IsValid(Actor))
				Actor->Destroy();
		}
		Pool.ActiveActors.Empty();
		Pool.ActiveOrderList->Empty();
		Pool.ActiveNodeMap->Empty();
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

	// ReadyActors에 쓸 수 있는 게 있으면 쓴다.
	Spawned = GetReadyActor(Pool);
	if (Spawned)
	{
		Spawned->SetActorTransform(InTransform);
		UE_LOG(LogTemp, Log, TEXT("Spawn(Reuse) : %s"), Spawned ? *Spawned->GetName() : TEXT("None"));
	}
	else
	{
		// ReadyActors에서 쓸 수 있는 것이 없다 => 새로 만들기
			const uint32 TotalCount = Pool->ActiveActors.Num() + Pool->ReadyActors.Num();
			const bool bMax = TotalCount >= static_cast<uint32>(Pool->MaxSize);

			if (!bMax)
			{
				Spawned = CreateNewObject(InClassType, InTransform);
				UE_LOG(LogTemp, Log, TEXT("Spawn(New) : %s"), Spawned ? *Spawned->GetName() : TEXT("None"));
			}
			else
			{
				switch (Pool->MaxPolicy)
				{
				case EObjectPoolPolicy::DoNotSpawn :
					return nullptr;	// 스폰 안 하고 바로 끝내기

				case EObjectPoolPolicy::Grow :
					CreateNewObject(InClassType, InTransform);
					UE_LOG(LogTemp, Log, TEXT("Spawn(New) : %s"), Spawned ? *Spawned->GetName() : TEXT("None"));
					break;

				case EObjectPoolPolicy::ReuseOldest :
					if (FOrderNode* HeadNode = Pool->ActiveOrderList->GetHead())
					{
						AActor* OldestActor = HeadNode->GetValue();
						ReturnPool(OldestActor);		// 편의성 + 리셋을 위해 ReturnPool 먼저 수행
						Spawned = GetReadyActor(Pool);
						Spawned->SetActorTransform(InTransform);
						
						UE_LOG(LogTemp, Log, TEXT("Spawn(Oldest) : %s"), Spawned ? *Spawned->GetName() : TEXT("None"));
					}
					break;

				default:
					break;
				}
			}
	}
	
	if (Spawned)
	{
		// ReadyActors에서 꺼냈든 새로 만들었든 스폰됐을 대 해야 할 일 처리
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
		FOrderNode* NewNode = new FOrderNode(Spawned);
		Pool->ActiveOrderList->AddTail(NewNode);
		Pool->ActiveNodeMap->Add(Spawned, NewNode);
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
	if (FOrderNode** FoundNode = Pool->ActiveNodeMap->Find(InActor))
	{
		Pool->ActiveOrderList->RemoveNode(*FoundNode, true);	// 리스트에서 FoundNode 제거하고 delete까지 처리
		Pool->ActiveNodeMap->Remove(InActor);
	}

	Pool->ReadyActors.Add(InActor);
}

AActor* UObjectPoolSubsystem::CreateNewObject(TSubclassOf<AActor> InClassType, const FTransform& InTransform)
{
	if (!GetWorld())
		return nullptr;

	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = nullptr;
	SpawnParam.ObjectFlags = RF_Transient;

	AActor* Spawned = GetWorld()->SpawnActor<AActor>(InClassType, InTransform, SpawnParam);
	UE_LOG(LogTemp, Log, TEXT("Spawn(New) : %s"), Spawned ? *Spawned->GetName() : TEXT("None"));

#if WITH_EDITOR
	if (Spawned)
	{
		Spawned->SetFolderPath(FName("Pool"));	// 아웃라이너에 표시될 폴더 정리
	}
#endif

	return Spawned;
}

AActor* UObjectPoolSubsystem::GetReadyActor(FObjectPool* InPool)
{
	if (!InPool)
		return nullptr;

	AActor* ReadyActor = nullptr;

	while (InPool->ReadyActors.Num() > 0)
	{
		// ReadyActors에서 쓸 수 있는 것이 있다 => 꺼내 쓰기
		AActor* Candidate = InPool->ReadyActors.Pop();	// 뒤에서 꺼내기
		if (IsValid(Candidate))
		{
			ReadyActor = Candidate;
			break;
		}
	}

	return ReadyActor;
}
