// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PreGetJsonSync.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UPreGetJsonSync : public UInterface
{
	GENERATED_BODY()
};

/**
 * This interface can be implemented to any actor. If implemented, before exporting the json's the Actor has an opportunity to Sync, therefore updating its data to be relevant
 */
class WORLDTOJSONEXPORTER_API IPreGetJsonSync
{
	GENERATED_BODY()
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Json|Export")
	void Sync();
};
