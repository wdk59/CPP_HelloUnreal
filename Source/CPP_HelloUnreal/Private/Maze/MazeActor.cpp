// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze/MazeActor.h"
#include "Maze/CellActor.h"
#include "Maze/MazeData.h"

// Sets default values
AMazeActor::AMazeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void AMazeActor::ClearMaze()
{
	// Maze의 셀 전부 삭제
	for (TObjectPtr<ACellActor> Cell : Cells)
	{
		if (IsValid(Cell))
		{
			Cell->Destroy();
		}
	}

	Cells.Empty();
}

// Called when the game starts or when spawned
void AMazeActor::BeginPlay()
{
	Super::BeginPlay();

	ClearMaze();
	
}

// Called every frame
void AMazeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMazeActor::GenerateMaze()
{
	OnGenerateMaze();
}

void AMazeActor::OnGenerateMaze()
{
	if (!CellActorClassType)
	{
		UE_LOG(LogTemp, Error, TEXT("Error: Maze를 생성할 Cell 지정 필요"));
		return;
	}

	if (MazeWidth * MazeHeight <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Error: Maze를 생성할 수 없는 크기 설정"));
		return;
	}

	ClearMaze();

	FMazeData MazeData;

	MazeData.MakeMaze(MazeWidth, MazeHeight, 0);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	for (int32 y = 0; y < MazeHeight; y++)
	{
		for (int32 x = 0; x < MazeWidth; x++)
		{
			if (UWorld* World = GetWorld())
			{
				if (ACellActor* SpawnedCell = World->SpawnActor<ACellActor>(CellActorClassType, this->GetActorTransform()))
				{
					SpawnedCell->SetActorLocation(FVector(-(y * SpawnedCell->GetCellSize()), x * SpawnedCell->GetCellSize(), 0.f));
					SpawnedCell->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
					SpawnedCell->InitializeCell(MazeData.GetCell(x, y));
					Cells.Add(SpawnedCell);
				}
			}
		}
	}

	MazeData.ClearMaze();
}
