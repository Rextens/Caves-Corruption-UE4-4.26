// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilites.h"

UObject* UUtilites::duplicateObject(UObject* reference, UObject *owner)
{
	return DuplicateObject<UObject>(reference, owner);
}

void UUtilites::swapItemPointers(UItem* firstItem, UItem* secondItem)
{
	//std::swap(firstItem, secondItem);
	UItem* temp = firstItem;
	firstItem = secondItem;
	secondItem = temp;
}

TArray<FString> UUtilites::loadAllFilesFromDirectory(FString directory, FString extension)
{
	TArray<FString> locatedFiles;
	const TCHAR* tempDirectory = *directory;
	const TCHAR* tempExtension = *extension;

	IFileManager &files = IFileManager::Get();
	files.FindFiles(locatedFiles, tempDirectory, tempExtension);

	TArray<FString> loadedRecipes;
	

	for (int i = 0; i < locatedFiles.Num(); ++i)
	{
		printString(directory + locatedFiles[i]);
		FString result;
		FFileHelper::LoadFileToString(result, *(directory + locatedFiles[i]));
		loadedRecipes.Add(result);
	}

	return loadedRecipes;
}

void UUtilites::printString(FString text, int32 duration )
{
	UKismetSystemLibrary::PrintString(GEngine->GetWorld(), text, true, false, FLinearColor::Red, duration);
}