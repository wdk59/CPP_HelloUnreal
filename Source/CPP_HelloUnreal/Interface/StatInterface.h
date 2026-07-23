// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StatInterface.generated.h"

class UStatComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UStatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CPP_HELLOUNREAL_API IStatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UStatComponent* GetStatComponent() const;

};
