// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "JsonObjectWrapper.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WorldToJsonExportBPLibrary.generated.h"

/**
 * 
 */
UCLASS()
class WORLDTOJSONEXPORTER_API UWorldToJsonExportBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public: 
	/// Create json attributes for the World currently open editor
	/// @param WorldJson
	/// @param WorldName
	/// @param bOutSuccess 
	/// @param Mask
	/// @param FieldNameCharacterLimit
	UFUNCTION(BlueprintCallable, Category="Json|Export")
	static void CreateJsonForCurrentWorld(FJsonObjectWrapper& WorldJson, FString& WorldName, bool& bOutSuccess, UPARAM(meta=(Bitmask)) const int32& Mask, const int& FieldNameCharacterLimit);

	/// Create json attributes for a World
	/// @param World 
	/// @param WorldJson 
	/// @param bOutSuccess 
	/// @param OutInfoMessage
	/// @param Mask
	/// @param FieldNameCharacterLimit
	UFUNCTION(BlueprintCallable, Category="Json|Export")
	static void CreateJsonForWorld(const UWorld* World, FJsonObjectWrapper& WorldJson, bool& bOutSuccess,
	                               UPARAM(meta=(Bitmask)) const int32& Mask, const int& FieldNameCharacterLimit);


	/// Create json attributes for an Actor
	/// This will iterate through available components attached to the actor.
	/// This will NOT iterate in Actors attached to the Actor
	/// @param Actor 
	/// @param WorldJson 
	/// @param bOutSuccess 
	/// @param OutInfoMessage
	/// @param Mask
	/// @param FieldNameCharacterLimit
	UFUNCTION(BlueprintCallable, Category="Json|Export")
	static void CreateJsonForActor(AActor* Actor, FJsonObjectWrapper& WorldJson, bool& bOutSuccess,
	                               FString& OutInfoMessage, UPARAM(meta=(Bitmask)) const int32& Mask, const int& FieldNameCharacterLimit);

	/// Create Json Attributes for a Scene Component
	/// This will iterate though Child Components of the Scene Component
	/// @param Component 
	/// @param ParentJson
	/// @param WorldJson 
	/// @param bOutSuccess 
	/// @param OutInfoMessage 
	/// @param ProcessedComponents - Keeps track which children have been solved in this particular iteration
	/// @param Mask
	/// @param FieldNameCharacterLimit
	UFUNCTION(BlueprintCallable, Category="Json|Export")
	static void CreateJsonForSceneComponent(const USceneComponent* Component,
	                                        FJsonObjectWrapper& ParentJson, FJsonObjectWrapper& WorldJson,
	                                        bool& bOutSuccess, FString& OutInfoMessage,
	                                        TArray<USceneComponent*>& ProcessedComponents, UPARAM(meta=(Bitmask)) const int32& Mask, const int& FieldNameCharacterLimit);

	

	UFUNCTION(BlueprintCallable, Category="Json|Export")
	static bool SaveCurrentWorldToJson(const FString Directory, const FString FileName, const int32& Mask, const int& FieldNameCharacterLimit, const bool NameHasWorldAsPrefix = true, bool AutoOpenDirectory = true);
	
	UFUNCTION(BlueprintCallable, Category="Json|Export")
	static bool SaveWorldToJson(const UWorld* World, const FString Directory, const FString FileName, const int32& Mask, const int& FieldNameCharacterLimit, const bool NameHasWorldAsPrefix = true, bool AutoOpenDirectory = true);
};
