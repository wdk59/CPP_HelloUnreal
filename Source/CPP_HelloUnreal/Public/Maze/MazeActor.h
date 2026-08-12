// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Maze/MazeData.h"
#include "MazeActor.generated.h"

struct FCellData;
class ACellActor;
class UHierarchicalInstancedStaticMeshComponent;

UCLASS()
class CPP_HELLOUNREAL_API AMazeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMazeActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected :

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze", meta = (ClampMin = "3", ClampMax = "100"))
	int32 Width = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze", meta = (ClampMin = "3", ClampMax = "100"))
	int32 Height = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze")
	int32 RandomSeed = FMazeData::RandomSeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Performance")
	bool bUseHISM = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|HISM")
	float CellHalfSize = 1000.f;


	// CellActor 스폰 방식 ----------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Maze|Spawn")
	TSubclassOf<ACellActor> CellActorClass = nullptr;

	UPROPERTY(VisibleInstanceOnly, Category = "Maze|Spawn")
	TArray<TObjectPtr<ACellActor>> SpawnedCells;
	// ------------------------------------------------------------------------------
	
	// HISM 사용 방식 ---------------------------------------------------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Maze|Components")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> FloorHISM = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Maze|Components")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> WallHISM = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Maze|Components")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> GateHISM = nullptr;
	// ------------------------------------------------------------------------------

protected :

	UFUNCTION(CallInEditor, Category = "Maze")
	void GenerateMaze();

	UFUNCTION(CallInEditor, Category = "Maze")
	void ClearMaze();

	// Nav Mesh 업데이트 최적화 위한 함수
	virtual void OnPreMazeGenerate();
	virtual void OnPostMazeGenerate();

private :

	// MazeData 생성
	TUniquePtr<FMazeData> MakeMazeData();

	// CellActor 스폰 방식 ----------------------------------------------------------
	// 각 Cell 생성
	void SpawnCells(FMazeData* InMaze);
	void ClearSpanwedCells();
	// ------------------------------------------------------------------------------

	// HISM 사용 방식 ---------------------------------------------------------------
	void BuildMazeHISM(FMazeData* InMaze);
	void ClearHISMInstances();
	void MakeCellHISM(const FCellData* InCell, const FVector& InLocation);
	// ------------------------------------------------------------------------------

};
