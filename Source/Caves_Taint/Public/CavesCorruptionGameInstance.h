// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CavesCorruptionGameInstance.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class CAVES_TAINT_API UCavesCorruptionGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FString> loadedCraftingRecipes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString currentWorldName;
};
