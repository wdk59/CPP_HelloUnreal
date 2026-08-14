// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryCommandTypes.generated.h"

UENUM(BlueprintType)
enum class EInventoryCommandType : uint8
{
	None,
	Add		// 아이템 획득
};

class UItemDataAsset;

USTRUCT(BlueprintType)
struct CPP_HELLOUNREAL_API FInventoryCommand
{
	GENERATED_BODY()

public :

	UPROPERTY(BlueprintReadWrite, Category = "Inventory|Command")
	EInventoryCommandType Type = EInventoryCommandType::None;

	UPROPERTY(BlueprintReadWrite, Category = "Inventory|Command")
	int32 Count = 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "Inventory|Command")
	TObjectPtr<const UItemDataAsset> ItemData = nullptr;

	//UPROPERTY(BlueprintReadWrite, Category = "Inventory|Command")
	//int32 SourceIndex = 1;
	//
	//UPROPERTY(BlueprintReadWrite, Category = "Inventory|Command")
	//int32 TargetIndex = 1;

	//UPROPERTY(BlueprintReadWrite, Category = "Inventory|Command")
	//FVector2D ScreenPosition = FVector2D::ZeroVector;

public :
	// 커맨드 생성용 static 헬퍼 함수

	// Add 커맨드 생성 함수
	static FInventoryCommand MakeAdd(UItemDataAsset* InItemData, int32 InCount = 1)
	{
		FInventoryCommand Command;
		Command.Type = EInventoryCommandType::Add;
		Command.ItemData = InItemData;
		Command.Count = InCount;

		return Command;
	}

};

USTRUCT(BlueprintType)
struct CPP_HELLOUNREAL_API FInventoryCommandResult
{
	GENERATED_BODY()

	// 커맨드 성공 여부
	UPROPERTY(BlueprintReadOnly)
	bool bSuccess = false;

	// Add 커맨드용 변수 (인벤토리에 들어가지 못한 아이템 개수)
	UPROPERTY(BlueprintReadOnly)
	int32 RemainingCount = 0;
};
