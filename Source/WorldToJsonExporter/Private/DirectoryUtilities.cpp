// Fill out your copyright notice in the Description page of Project Settings.


#include "DirectoryUtilities.h"

#include "JsonBlueprintFunctionLibrary.h"

bool UDirectoryUtilities::OpenDirectory(const FFilePath& File)
{
	if (!ValidateDirectory(File)) return false;

	if (File.FilePath.IsEmpty())
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("FileName cannot be empty")), ELogVerbosity::Error);
		return false;
	}

	FPlatformProcess::ExploreFolder(*File.FilePath);
	return true;
}

bool UDirectoryUtilities::ValidateDirectory(const FFilePath& File)
{
	if (!FPaths::DirectoryExists(FPaths::GetPath(File.FilePath)))
	{
		FFrame::KismetExecutionMessage(
			*FString::Printf(TEXT("Directory not found: %s"), *FPaths::GetPath(File.FilePath)),
			ELogVerbosity::Error);
		return false;
	}
	return true;
}