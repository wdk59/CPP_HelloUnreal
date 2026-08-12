// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze/MazeActor.h"
#include "Maze/CellActor.h"

#include "NavigationSystem.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
AMazeActor::AMazeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(Root);

	FloorHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("FloorHISM"));
	FloorHISM->SetupAttachment(Root);
	FloorHISM->SetCollisionProfileName(TEXT("BlockAll"));

	WallHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("WallHISM"));
	WallHISM->SetupAttachment(Root);
	WallHISM->SetCollisionProfileName(TEXT("BlockAll"));

	GateHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("GateHISM"));
	GateHISM->SetupAttachment(Root);
	GateHISM->SetCollisionProfileName(TEXT("BlockAllDynamic"));

}

// Called when the game starts or when spawned
void AMazeActor::BeginPlay()
{
	Super::BeginPlay();
	
	GenerateMaze();

}

// Called every frame
void AMazeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMazeActor::GenerateMaze()
{
	if (Width * Height <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("생성할 수 없는 미로 크기입니다. 명령을 수행하지 않습니다."));

		return;
	}

	OnPreMazeGenerate();

	ClearMaze();	// 이전에 만들어진 미로가 있으면 삭제

	TUniquePtr<FMazeData> Maze = MakeMazeData();

	if (bUseHISM)
	{
		BuildMazeHISM(Maze.Get());
	}
	else
	{
		SpawnCells(Maze.Get());
	}

	OnPostMazeGenerate();
}

void AMazeActor::ClearMaze()
{
	if (bUseHISM)
	{
		ClearHISMInstances();
	}
	else
	{
		ClearSpanwedCells();
	}

}

void AMazeActor::OnPreMazeGenerate()
{
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSystem)
	{
		NavSystem->SetNavigationAutoUpdateEnabled(false, NavSystem);	// NavMesh 자동 업데이트 막기
	}
}

void AMazeActor::OnPostMazeGenerate()
{
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSystem)
	{
		NavSystem->Build();	// NavMesh 수동 생성
		NavSystem->SetNavigationAutoUpdateEnabled(true, NavSystem);	// NavMesh 자동 업데이트 재개
	}
}

TUniquePtr<FMazeData> AMazeActor::MakeMazeData()
{
	TUniquePtr<FMazeData> Maze = MakeUnique<FMazeData>();
	Maze->MakeMaze(static_cast<uint8>(Width), static_cast<uint8>(Height), RandomSeed);

	return Maze;
}

void AMazeActor::SpawnCells(FMazeData* InMaze)
{
	if (!CellActorClass)
		return;

	SpawnedCells.Reserve(Width * Height);

	const ACellActor* DefaultCell = CellActorClass->GetDefaultObject<ACellActor>();

	CellHalfSize = DefaultCell->GetCellHalfSize();
	float CellSize = CellHalfSize * 2.f;

	FVector StartLocation = FVector(Height * CellHalfSize, -Width * CellHalfSize, 0.f)
		+ FVector((Height % 2) * -CellHalfSize, (Width % 2) * CellHalfSize, 0.f);

	UWorld* World = GetWorld();
	if (!World)
		return;

	// x, y는 MazeData의 좌표계 기준
	for (uint8 y = 0; y < Height; y++)		// y는 남쪽(뒷쪽)으로 증가함을 나타냄
	{
		for (uint8 x = 0; x < Width; x++)	// x는 동쪽(오른쪽)으로 증가함을 나타냄
		{
			FCellData* Cell = InMaze->GetCell(x, y);
			if (!Cell)
				continue;

			FVector CellLocation = StartLocation + FVector(-y * CellSize, x * CellSize, 0.f);
			ACellActor* CellActor = World->SpawnActor<ACellActor>(
				CellActorClass,
				CellLocation,
				FRotator::ZeroRotator
			);

			if (CellActor)
			{
				CellActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
				CellActor->InitializeCell(Cell);
				SpawnedCells.Add(CellActor);
			}
		}
	}
}

void AMazeActor::ClearSpanwedCells()
{
	for (ACellActor* Cell : SpawnedCells)
	{
		if (IsValid(Cell))
		{
			Cell->Destroy();
		}
	}
	SpawnedCells.Empty();
}

void AMazeActor::BuildMazeHISM(FMazeData* InMaze)
{
	if (!InMaze)
		return;

	if (!FloorHISM->GetStaticMesh() || !WallHISM->GetStaticMesh() || !GateHISM->GetStaticMesh())
	{
		UE_LOG(LogTemp, Log, TEXT("HISM 컴포넌트에 스태틱 메시가 없습니다."));
		return;
	}

	ClearHISMInstances();
	
	float CellSize = CellHalfSize * 2.f;
	FVector StartLocation = FVector(Height * CellHalfSize, -Width * CellHalfSize, 0.f)
		+ FVector((Height % 2) * -CellHalfSize, (Width % 2) * CellHalfSize, 0.f);

	for (uint8 y = 0; y < Height; y++)		// y는 남쪽(뒷쪽)으로 증가함을 나타냄
	{
		for (uint8 x = 0; x < Width; x++)	// x는 동쪽(오른쪽)으로 증가함을 나타냄
		{
			FCellData* Cell = InMaze->GetCell(x, y);
			if (!Cell)
				continue;

			// 셀의 위치 결정
			FVector CellLocation = StartLocation + FVector(-y * CellSize, x * CellSize, 0.f);
			MakeCellHISM(Cell, CellLocation);
		}
	}
}

void AMazeActor::ClearHISMInstances()
{
	if (FloorHISM)
	{
		FloorHISM->ClearInstances();
	}
	if (WallHISM)
	{
		WallHISM->ClearInstances();
	}
	if (GateHISM)
	{
		GateHISM->ClearInstances();
	}
	FlushPersistentDebugLines(GetWorld());	// 디버그 박스 지우기
}

void AMazeActor::MakeCellHISM(const FCellData* InCell, const FVector& InLocation)
{
	if (!InCell)
		return;

	// 바닥 추가
	FloorHISM->AddInstance(FTransform(FRotator::ZeroRotator, InLocation));

	// 벽과 문 추가
	FVector Location;
	FRotator Rotate;

	Location = InLocation + FVector::ForwardVector * CellHalfSize;
	Rotate = FRotator::ZeroRotator;
	WallHISM->AddInstance(FTransform(Rotate, Location));	// 벽 만들기
	if (InCell->IsWall(EDirectionType::North))	// 문 만들기
	{
		GateHISM->AddInstance(FTransform(Rotate, Location));
	}

	Location = InLocation + FVector::LeftVector * CellHalfSize;
	Rotate = FRotator(0.f, -90.f, 0.f);
	WallHISM->AddInstance(FTransform(Rotate, Location));	// 벽 만들기
	if (InCell->IsWall(EDirectionType::West))	// 문 만들기
	{
		GateHISM->AddInstance(FTransform(Rotate, Location));
	}

	if (InCell->X == Width - 1)	// 동쪽 끝
	{
		Location = InLocation + FVector::RightVector * CellHalfSize;
		Rotate = FRotator(0.f, 90.f, 0.f);
		WallHISM->AddInstance(FTransform(Rotate, Location));
		GateHISM->AddInstance(FTransform(Rotate, Location));	// 동쪽 끝은 막혀 있는 것이 확정
	}

	if (InCell->Y == Height - 1)	// 남쪽 끝
	{
		Location  = InLocation + FVector::BackwardVector * CellHalfSize;
		Rotate = FRotator(0.f, 180.f, 0.f);
		WallHISM->AddInstance(FTransform(Rotate, Location));
		GateHISM->AddInstance(FTransform(Rotate, Location));	// 남쪽 끝은 막혀 있는 것이 확정
	}

#if WITH_EDITOR
	DrawDebugBox(
		GetWorld(),
		InLocation,
		FVector(CellHalfSize, CellHalfSize, 400.f),
		FColor::Green,
		true,
		-1.f,
		0,
		3.f
	);
#endif

}
