#pragma once

#include "CoreMinimal.h"
#include "JsonObjectWrapper.h"
#include "Components/ActorComponent.h"
#include "JsonExportComponent.generated.h"

UCLASS(BlueprintType, Blueprintable, ClassGroup=(JsonAttribute), meta=(BlueprintSpawnableComponent))
class WORLDTOJSONEXPORTER_API UJsonExportComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UJsonExportComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Json|Export")
	FString FieldName = "JsonFieldName";

	/* Will create an empty Json object for FieldName.
	 * If true, will create an empty json object for other json objects to be parented to
	 * This is known as a 'Wrapper'
	 * Any implementation of GetJson will be ignored */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Json|Export")
	bool bIsWrapper = false;

	/* Will parent json objects automatically based on wrapper hierarchy.
	 * If true, this json object will be parented to the first Wrapper found in its parent hierarchy
	 * If false, this json will be parented directly to the base json object */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Json|Export")
	bool bAutoParent = true;

	/* Whether or not it will export a json object (including children, if Wrapper) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Json|Export")
	bool bEnabled = true;

	/* Export call will provide a JsonMask. If both masks have any overlapping bits, the json object will be exported.
	 * If not, the entire json object (including children if its a Wrapper) will be ignored.
	 */
	UPROPERTY(EditAnywhere, Category="Json|Export", Meta = (Bitmask))
	int32 JsonMask = 1;

	/* Sets the JsonField from the Blueprint Implementation */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Json|Export")
	void SetJsonField(const FJsonObjectWrapper& Json, const FString& JsonFieldName, bool& bOutSuccess, FString& OutMessage) const;
};
