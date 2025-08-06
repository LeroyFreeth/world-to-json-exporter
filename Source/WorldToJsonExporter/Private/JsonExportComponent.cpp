// Fill out your copyright notice in the Description page of Project Settings.

#include "JsonExportComponent.h"
#include "WorldToJsonExporter.h"

// Sets default values for this component's properties
UJsonExportComponent::UJsonExportComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

/// Be sure to cache the Json first, as it goes out of scope in BPs. 
/// @param Json
/// @param JsonFieldName
/// @param bOutSuccess
/// @param OutMessage 
void UJsonExportComponent::SetJsonField_Implementation(const FJsonObjectWrapper& Json, const FString& JsonFieldName,
                                                  bool& bOutSuccess, FString& OutMessage) const
{
	bOutSuccess = true;
}
