// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MazeActor.generated.h"

class ACellActor;

UCLASS()
class CPP_HELLOUNREAL_API AMazeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMazeActor();

	// 생성된 미로 제거
	void ClearMaze();
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected :
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Generate")
	TSubclassOf<ACellActor> CellActorClassType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<ACellActor>> Cells;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 MazeSeed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Size")
	uint8 MazeWidth = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Size")
	uint8 MazeHeight = 0;

protected :

	// Maze 생성 버튼을 위한 함수
	UFUNCTION(CallInEditor, Category = "Maze|Generate")
	void GenerateMaze();

	// Maze 생성 명령이 떨어졌을 때 실제로 Maze를 생성할 함수
	void OnGenerateMaze();

};
