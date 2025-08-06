// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectWrapper.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonUtilityBPLibrary.generated.h"

/**
 * 
 */
UCLASS()
class WORLDTOJSONEXPORTER_API UJsonUtilityBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Json|Utilities")
	static FJsonObjectWrapper WrapJsonObjectContent(const FString& FieldName, const FJsonObjectWrapper& JsonObject);

	UFUNCTION(BlueprintCallable, Category="Json|Export")
	static bool Make2DFloatJsonObject(const FJsonObjectWrapper& JsonObject, const FString& FieldName, const TArray<float>& Values,
																const TArray<int>& SectionSizes);


	UFUNCTION(BlueprintCallable, Category="Json|Utilities")
	static FJsonObjectWrapper ChangeFieldName(const FJsonObjectWrapper& JsonObject, const FString& OldFieldName,
	                                          const FString& NewFieldName);

	UFUNCTION(BlueprintCallable, Category="Json|Utilities")
	static bool IsJsonObject(const FJsonObjectWrapper& JsonObjectWrapper, const FString& FieldName,
	                         FJsonObjectWrapper& FoundJsonObjectWrapper);

	UFUNCTION(BlueprintCallable, Category="Json|Utilities")
	static FJsonObjectWrapper SortFields(const FJsonObjectWrapper& JsonObject);

	UFUNCTION(BlueprintCallable, Category="String|Utilities")
	static TArray<int> GetNumbersFromString(const FString& SourceString);

	UFUNCTION(BlueprintCallable, Category="Json|Utilities")
	static FJsonObjectWrapper CombineJsonObjects(const FJsonObjectWrapper& A, const FJsonObjectWrapper& B);

	UFUNCTION(BlueprintCallable, Category="String|Utilities")
	static bool Compare(FString A, FString B);
};
