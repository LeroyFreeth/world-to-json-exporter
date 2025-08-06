// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectWrapper.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DirectoryUtilities.generated.h"

/**
 * 
 */
UCLASS()
class WORLDTOJSONEXPORTER_API UDirectoryUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Directory|Utilities")
	static bool OpenDirectory(const FFilePath& File);
	UFUNCTION(BlueprintCallable, Category="Directory|Utilities")
	static bool ValidateDirectory(const FFilePath& File);
};
