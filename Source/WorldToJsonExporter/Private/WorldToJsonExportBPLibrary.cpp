// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldToJsonExportBPLibrary.h"
#include <cwctype>

#include "DirectoryUtilities.h"
#include "EngineUtils.h"
#include "JsonExportComponent.h"
#include "WorldToJsonExporter.h"
#include "EditorDialogLibrary.h"
#include "JsonBlueprintFunctionLibrary.h"
#include "PreGetJsonSync.h"

void UWorldToJsonExportBPLibrary::CreateJsonForCurrentWorld(
	FJsonObjectWrapper& WorldJson, FString& WorldName, bool& bOutSuccess,
	const int32& Mask, const int& FieldNameCharacterLimit)
{
	const FWorldContext* WorldContext = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	const UWorld* World = WorldContext->World();
	CreateJsonForWorld(World, WorldJson, bOutSuccess, Mask, FieldNameCharacterLimit);
	WorldName = World->GetName();
}

void UWorldToJsonExportBPLibrary::CreateJsonForWorld(const UWorld* World, FJsonObjectWrapper& WorldJson,
                                                     bool& bOutSuccess, const int32& Mask,
                                                     const int& FieldNameCharacterLimit)
{
	bOutSuccess = true;
	FString OutInfoMessage = FString::Printf(TEXT(""));
	if (!IsValid(World))
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("Cannot create attribute collection for World - World was not valid"));
		return;
	}

	UE_LOG(LogJsonExport, Log, TEXT("Creating Json for World - %s - With mask value %i"), *World->GetName(), Mask);

	TArray<AActor*> Actors;
	for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr) Actors.Push(*ActorItr);
	Actors.Sort([](const AActor& Left, const AActor& Right)
	{
		return Left.GetActorNameOrLabel() < Right.GetActorNameOrLabel();
	});

	for (AActor* Actor : Actors)
	{
		CreateJsonForActor(Actor, WorldJson, bOutSuccess, OutInfoMessage, Mask, FieldNameCharacterLimit);
	}

	// Validate WorldJson has any values
	if (WorldJson.JsonObject->Values.Num() <= 0)
	{
		// If not, export failed due to having no data to export
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(
			TEXT("Failure: Json export for World is empty - %s"), *World->GetDebugDisplayName());
	}

	if (bOutSuccess)
	{
		OutInfoMessage = FString::Printf(
			TEXT("Success: Json export for World - %s"), *World->GetDebugDisplayName());
		// Log
		UE_LOG(LogJsonExport, Log, TEXT("%s"), *OutInfoMessage)
	}
	else
	{
		// Warn
		UE_LOG(LogJsonExport, Warning, TEXT("%s"), *OutInfoMessage)
		// Unsuccessful, show message dialogue window
		FText Title = FText::FromString("Failed to export Json");
		FText Message = FText::FromString(*OutInfoMessage);
		TEnumAsByte MessageType = EAppMsgType::Type::Ok;
		TEnumAsByte ReturnType = EAppReturnType::Type::Ok;
		UEditorDialogLibrary::ShowMessage(Title, Message, MessageType, ReturnType);
	}
}

void UWorldToJsonExportBPLibrary::CreateJsonForActor(AActor* Actor, FJsonObjectWrapper& WorldJson,
                                                     bool& bOutSuccess, FString& OutInfoMessage, const int32& Mask,
                                                     const int& FieldNameCharacterLimit)
{
	if (!IsValid(Actor))
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("Cannot create attribute collection for Actor - Actor was not valid"));
		return;
	}

	// Loop through child actors
	TArray<AActor*> ChildActors;
	Actor->GetAllChildActors(ChildActors, true);
	for (const auto ChildActor : ChildActors)
		CreateJsonForActor(ChildActor, WorldJson, bOutSuccess, OutInfoMessage, Mask, FieldNameCharacterLimit);

	// Syncs actors which have the interface implemented
	// ScriptGuard allows call to be done in editor
	if (FEditorScriptExecutionGuard ScriptGuard; Actor->Implements<UPreGetJsonSync>())
	{
		IPreGetJsonSync::Execute_Sync(Actor);
	}

	TArray<USceneComponent*> Components;
	Actor->GetComponents(Components, false);
	//Algo::Reverse(Components);
	TArray<USceneComponent*> ProcessedComponents;
	for (const auto Component : Components)
	{
		// Only cycle through the first components to dictate iteration order from top to bottom
		if (Component->GetAttachParent() != Actor->GetRootComponent()) continue;
		CreateJsonForSceneComponent(Component, WorldJson, WorldJson, bOutSuccess,
		                            OutInfoMessage, ProcessedComponents, Mask, FieldNameCharacterLimit);
	}
	ProcessedComponents.Empty();
}

void UWorldToJsonExportBPLibrary::CreateJsonForSceneComponent(const USceneComponent* Component,
                                                              FJsonObjectWrapper& ParentJson,
                                                              FJsonObjectWrapper& WorldJson,
                                                              bool& bOutSuccess,
                                                              FString& OutInfoMessage,
                                                              TArray<USceneComponent*>&
                                                              ProcessedComponents,
                                                              const int32& Mask, const int& FieldNameCharacterLimit)
{
	// Validation
	if (!IsValid(Component))
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(
			TEXT("Cannot create attribute collection for Component - Component was not valid"));
		return;
	}

	// Cannot process a component twice
	if (ProcessedComponents.Contains(Component)) return;
	ProcessedComponents.Push(const_cast<TArray<USceneComponent*>::ElementType>(Component));

	// Create default JsonWrapper
	FJsonObjectWrapper JsonWrapper = FJsonObjectWrapper();
	const TSharedRef<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonWrapper.JsonObject = JsonObject;

	bool IsWrapper = false;
	bool AutoParent = true;
	FString WrapperFieldName;

	// Setup children array
	TArray<USceneComponent*> Children;
	Component->GetChildrenComponents(true, Children);

	// Check if component is JsonExportComponent
	if (Component->IsA(UJsonExportComponent::StaticClass()))
	{
		if (const UJsonExportComponent* JsonExportComponent = Cast<UJsonExportComponent>(Component))
		{
			FString FieldName = JsonExportComponent->FieldName;
			// Check for empty field names
			if (FieldName.IsEmpty())
			{
				bOutSuccess = false;
				OutInfoMessage = FString::Printf(
					TEXT(
						"Failed: Exporting %s on Actor %s - FieldName is empty"),
					*JsonExportComponent->GetOwner()->GetActorNameOrLabel()
					, *JsonExportComponent->GetName());
				UE_LOG(LogJsonExport, Warning, TEXT("%s"), *OutInfoMessage)
			}

			// Check for character limit
			if (FieldName.Len() >= FieldNameCharacterLimit)
			{
				bOutSuccess = false;
				FString ShortenedName = FieldName.Mid(0, FieldNameCharacterLimit);
				FString DisplayName = JsonExportComponent->GetFullName();

				OutInfoMessage = FString::Printf(
					TEXT(
						"Failed: Exporting %s on Actor %s - FieldName too long - %s. Max characters is %i, suggest name - %s"
					), *JsonExportComponent->GetOwner()->GetActorNameOrLabel(), *JsonExportComponent->GetName(),
					*FieldName, FieldNameCharacterLimit, *ShortenedName);
				UE_LOG(LogJsonExport, Warning, TEXT("%s"), *OutInfoMessage)
			}

			// Cache
			IsWrapper = JsonExportComponent->bIsWrapper;
			AutoParent = JsonExportComponent->bAutoParent;
			WrapperFieldName = JsonExportComponent->FieldName;

			// Mask is valid if there is ANY overlap
			bool maskIsValid = (Mask & JsonExportComponent->JsonMask) != 0;
			if (!JsonExportComponent->bEnabled || !maskIsValid)
			{
				// If Wrapper, also consider all children processed
				if (IsWrapper) ProcessedComponents.Append(Children);
				return;
			}

			// Adds the json attribute to a nested json or the main json
			if (!JsonExportComponent->bIsWrapper)
			{
				FString BpOutInfoMessage = FString::Printf(TEXT(""));
				bool bBpOutSuccess = true;
				JsonExportComponent->SetJsonField(JsonWrapper, FieldName, bBpOutSuccess, BpOutInfoMessage);

				if (!bBpOutSuccess)
				{
					bOutSuccess = false;
					// Set some predefined structure on out message
					OutInfoMessage = FString::Printf(
						TEXT(
							"Failed: Exporting %s on Actor %s - %s"
						), *JsonExportComponent->GetOwner()->GetActorNameOrLabel(), *JsonExportComponent->GetName(),
						*BpOutInfoMessage);
					UE_LOG(LogJsonExport, Warning, TEXT("%s"), *OutInfoMessage)
				}
			}
		}
	}

	// Regardless whether component is of type JsonExportComponent, traverse through it.
	for (const auto Child : Children)
	{
		// Skip self
		if (Child == Component) continue;
		CreateJsonForSceneComponent(Child, JsonWrapper, WorldJson, bOutSuccess, OutInfoMessage,
		                            ProcessedComponents,
		                            Mask, FieldNameCharacterLimit);
	}
	const FJsonObjectWrapper& Parent = AutoParent ? ParentJson : WorldJson;

	if (IsWrapper)
	{
		// One more check to only export the wrapper if it has content
		if (JsonWrapper.JsonObject->Values.Num() > 0)
			Parent.JsonObject->SetField(WrapperFieldName,
			                            MakeShareable(
				                            new FJsonValueObject(JsonWrapper.JsonObject.ToSharedRef())));
	}
	else
	{
		for (auto Value : JsonWrapper.JsonObject->Values)
			Parent.JsonObject->SetField(Value.Key, Value.Value);
	}
	Children.Empty();
}

bool UWorldToJsonExportBPLibrary::SaveCurrentWorldToJson(const FString Directory, const FString FileName,
                                                         const int32& Mask, const int& FieldNameCharacterLimit,
                                                         const bool NameHasWorldAsPrefix, bool AutoOpenDirectory)
{
	const FWorldContext* WorldContext = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	const UWorld* World = WorldContext->World();
	return SaveWorldToJson(World, Directory, FileName, Mask, FieldNameCharacterLimit, NameHasWorldAsPrefix,
	                       AutoOpenDirectory);
}

bool UWorldToJsonExportBPLibrary::SaveWorldToJson(const UWorld* World, const FString Directory, const FString FileName,
                                                  const int32& Mask, const int& FieldNameCharacterLimit,
                                                  const bool NameHasWorldAsPrefix, bool AutoOpenDirectory)
{
	// Create Json object
	FJsonObjectWrapper WorldJson = FJsonObjectWrapper();
	// Creates a new nested JsonObject to pass through
	const TSharedRef<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	WorldJson.JsonObject = JsonObject;

	bool bOutSuccess = true;
	CreateJsonForWorld(World, WorldJson, bOutSuccess, Mask, FieldNameCharacterLimit);
	if (!bOutSuccess) return false;

	// Solve file path
	FFilePath FilePath;
	FilePath.FilePath = FString::Printf(TEXT("%s%s%s.json"),
	                                    *Directory,
	                                    NameHasWorldAsPrefix ? *World->GetName() : TEXT(""),
	                                    *FileName);

	// Try Save
	bOutSuccess = UJsonBlueprintFunctionLibrary::ToFile(WorldJson, FilePath);
	if (bOutSuccess && AutoOpenDirectory) UDirectoryUtilities::OpenDirectory(FilePath);
	return bOutSuccess;
}
