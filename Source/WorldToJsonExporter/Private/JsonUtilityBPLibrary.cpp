// Fill out your copyright notice in the Description page of Project Settings.

#include "JsonUtilityBPLibrary.h"
#include <cwctype>

#include "WorldToJsonExporter.h"

FJsonObjectWrapper UJsonUtilityBPLibrary::WrapJsonObjectContent(const FString& FieldName,
                                                                const FJsonObjectWrapper& JsonObject)
{
	const TSharedPtr<FJsonObject> WrapperObject = MakeShared<FJsonObject>();
	for (auto Value : JsonObject.JsonObject->Values)
	{
		switch (Value.Value->Type)
		{
		case EJson::None:
			break;
		case EJson::Null:
			break;
		case EJson::String:
			WrapperObject->SetStringField(Value.Key, JsonObject.JsonObject->GetStringField(Value.Key));
			break;
		case EJson::Number:
			WrapperObject->SetNumberField(Value.Key, JsonObject.JsonObject->GetNumberField(Value.Key));
			break;
		case EJson::Boolean:
			WrapperObject->SetBoolField(Value.Key, JsonObject.JsonObject->GetBoolField(Value.Key));
			break;
		case EJson::Array:
			WrapperObject->SetArrayField(Value.Key, JsonObject.JsonObject->GetArrayField(Value.Key));
			break;
		case EJson::Object:
			WrapperObject->SetObjectField(Value.Key, JsonObject.JsonObject->GetObjectField(Value.Key));
			break;
		}
	}
	const FJsonObjectWrapper Wrapper = FJsonObjectWrapper();
	Wrapper.JsonObject->SetObjectField(FieldName, WrapperObject);
	return Wrapper;
}

bool UJsonUtilityBPLibrary::Make2DFloatJsonObject(const FJsonObjectWrapper& JsonObject, const FString& FieldName, const TArray<float>& Values,
                                                                const TArray<int>& SectionSizes)
{
	TArray<TSharedPtr<FJsonValue>> JsonValueArrays;
	int Start = 0;
	for (int i = 0; i < SectionSizes.Num(); i++)
	{
		TArray<TSharedPtr<FJsonValue>> NestedValues;
		const int Length = SectionSizes[i];
		const int End = Start + Length;
		for (int j = Start; j < End; j++)
			NestedValues.Push(MakeShared<FJsonValueNumber>(FJsonValueNumber(Values[j])));
		Start += Length;
		JsonValueArrays.Push(MakeShared<FJsonValueArray>(NestedValues));
	}
	JsonObject.JsonObject->SetArrayField(FieldName, JsonValueArrays);
	return true;
}

FJsonObjectWrapper UJsonUtilityBPLibrary::ChangeFieldName(const FJsonObjectWrapper& JsonObject,
                                                          const FString& OldFieldName,
                                                          const FString& NewFieldName)
{
	if (!JsonObject.JsonObject->HasField(OldFieldName))
	{
		UE_LOG(LogJsonExport, Warning, TEXT("No field found on JsonObject - %s"), *OldFieldName)
		return JsonObject;
	}
	if (JsonObject.JsonObject->Values.Num() == 0)
		return JsonObject;

	const TSharedPtr<FJsonObject> NewJsonObject = MakeShared<FJsonObject>();

	for (auto Value : JsonObject.JsonObject->Values)
	{
		const FString Key = Value.Key == OldFieldName ? NewFieldName : Value.Key;

		switch (Value.Value->Type)
		{
		case EJson::None:
			break;
		case EJson::Null:
			break;
		case EJson::String:
			NewJsonObject->SetStringField(Key, JsonObject.JsonObject->GetStringField(Value.Key));
			break;
		case EJson::Number:
			NewJsonObject->SetNumberField(Key, JsonObject.JsonObject->GetNumberField(Value.Key));
			break;
		case EJson::Boolean:
			NewJsonObject->SetBoolField(Key, JsonObject.JsonObject->GetBoolField(Value.Key));
			break;
		case EJson::Array:
			NewJsonObject->SetArrayField(Key, JsonObject.JsonObject->GetArrayField(Value.Key));
			break;
		case EJson::Object:
			NewJsonObject->SetObjectField(Key, JsonObject.JsonObject->GetObjectField(Value.Key));
			break;
		}
	}
	const FJsonObjectWrapper Wrapper = FJsonObjectWrapper();
	Wrapper.JsonObject->Values = NewJsonObject->Values;
	return Wrapper;
}

bool UJsonUtilityBPLibrary::IsJsonObject(const FJsonObjectWrapper& JsonObjectWrapper, const FString& FieldName,
                                         FJsonObjectWrapper& FoundJsonObjectWrapper)
{
	const TSharedPtr<FJsonObject>* JsonObject = nullptr;
	bool isJsonObject = JsonObjectWrapper.JsonObject->TryGetObjectField(FieldName, JsonObject);
	if (!isJsonObject) return false;
	FoundJsonObjectWrapper.JsonObject = *JsonObject;
	return true;
}

/// Only has support for integers
/// @param SourceString 
TArray<int> UJsonUtilityBPLibrary::GetNumbersFromString(const FString& SourceString)
{
	TArray<int> numbers;
	const int Length = SourceString.Len();
	if (Length <= 0) return numbers;

	int bLastWasDigit = false;
	int bCreatingValue = false;

	int DecimalMultiplier = 1;
	int Value = 0;

	const TArray<wchar_t> Characters = SourceString.GetCharArray();
	for (int i = Length - 1; i >= 0; i--)
	{
		const wchar_t Char = Characters[i];
		const int bIsDigit = std::iswdigit(Char);
		if (bIsDigit)
		{
			if (bLastWasDigit) DecimalMultiplier *= 10;
			Value += (Char - 48) * DecimalMultiplier;
			bCreatingValue = true;
		}

		if (bCreatingValue && (!bIsDigit || i == 0))
		{
			numbers.Push(Value);
			Value = 0;
			DecimalMultiplier = 1;
			bCreatingValue = false;
		}

		bLastWasDigit = bIsDigit;
	}
	return numbers;
}

FJsonObjectWrapper UJsonUtilityBPLibrary::CombineJsonObjects(const FJsonObjectWrapper& A, const FJsonObjectWrapper& B)
{
	//const TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	TMap<FString, TSharedPtr<FJsonValue>> AllValues;
	AllValues.Append(A.JsonObject->Values);
	AllValues.Append(B.JsonObject->Values);
	const FJsonObjectWrapper Wrapper = FJsonObjectWrapper();
	Wrapper.JsonObject->Values = AllValues;
	return Wrapper;
}

bool UJsonUtilityBPLibrary::Compare(FString A, FString B)
{
	const TArray<wchar_t> CharactersA = A.GetCharArray();
	const TArray<wchar_t> CharactersB = A.GetCharArray();
	bool EndReached = false;
	int IndexA = 0;
	int IndexB = 0;

	while (!EndReached)
	{
		const wchar_t CharA = CharactersA[IndexA];
		const int bIsDigitA = std::iswdigit(CharA);
		IndexA++;
		if (IndexA == CharactersA.Num()) EndReached = true;

		const wchar_t CharB = CharactersB[IndexB];
		const int bIsDigitB = std::iswdigit(CharB);
		IndexB++;
		if (IndexB == CharactersA.Num()) EndReached = true;

		if (bIsDigitA || bIsDigitB) continue;

		// If not the same string without digits, do a normal comparison
		if (CharA != CharB) return A < B;
	}

	const auto NumbersA = GetNumbersFromString(A);
	const auto NumbersB = GetNumbersFromString(B);
	if (NumbersA.Num() == 0 || NumbersB.Num() == 0) return A < B;
	return (NumbersA[0] < NumbersB[0]);
}


FJsonObjectWrapper UJsonUtilityBPLibrary::SortFields(const FJsonObjectWrapper& JsonObject)
{
	const TSharedPtr<FJsonObject> NewJsonObject = MakeShared<FJsonObject>();
	TArray<FString> FieldNames;
	for (auto Value : JsonObject.JsonObject->Values)
		FieldNames.Push(Value.Key);

	FieldNames.Sort([](const FString& Left, const FString& Right)
	{
		return Compare(Left, Right);
	});

	for (auto FieldName : FieldNames)
	{
		TSharedPtr<FJsonValue> Value = JsonObject.JsonObject->Values[FieldName];
		switch (Value->Type)
		{
		case EJson::None:
			break;
		case EJson::Null:
			break;
		case EJson::String:
			NewJsonObject->SetStringField(FieldName, JsonObject.JsonObject->GetStringField(FieldName));
			break;
		case EJson::Number:
			NewJsonObject->SetNumberField(FieldName, JsonObject.JsonObject->GetNumberField(FieldName));
			break;
		case EJson::Boolean:
			NewJsonObject->SetBoolField(FieldName, JsonObject.JsonObject->GetBoolField(FieldName));
			break;
		case EJson::Array:
			NewJsonObject->SetArrayField(FieldName, JsonObject.JsonObject->GetArrayField(FieldName));
			break;
		case EJson::Object:
			NewJsonObject->SetObjectField(FieldName, JsonObject.JsonObject->GetObjectField(FieldName));
			break;
		}
	}
	const FJsonObjectWrapper Wrapper = FJsonObjectWrapper();
	Wrapper.JsonObject->Values = NewJsonObject->Values;
	return Wrapper;
}
