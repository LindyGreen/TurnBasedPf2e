#include "SaveSystem/PF2eSaveSystem.h"
#include "Kismet/GameplayStatics.h"

void UPF2eSaveSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
		LoadSave();//loads or creates a new save
}

bool UPF2eSaveSystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (this->IsInBlueprint())
	{
		return true;
	}
	return false;
}

bool UPF2eSaveSystem::SaveGame(UPF2eSaveFile* SaveFile)
{
	if (!SaveFile)
	{
		return false;
	}

	SaveFile->SaveDate = FDateTime::Now();
	return UGameplayStatics::SaveGameToSlot(SaveFile, SaveSlotName, 0);
}

UPF2eSaveFile* UPF2eSaveSystem::LoadSave()
{
	if (DoesSaveExist())
	{
		CurrentSaveFile=Cast<UPF2eSaveFile>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
		return CurrentSaveFile;
	};
	UPF2eSaveFile* NewSave = CreateNewSaveFile();
	SetDefaultCharacterValues(NewSave);
	CurrentSaveFile=NewSave;
	return NewSave;
}

bool UPF2eSaveSystem::DoesSaveExist()
{
	return UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0);
}

UPF2eSaveFile* UPF2eSaveSystem::CreateNewSaveFile()
{
	return NewObject<UPF2eSaveFile>();
}

UPF2eSaveFile* UPF2eSaveSystem::NewGame()
{
	// Delete existing save if it exists
	if (DoesSaveExist())
	{
		UGameplayStatics::DeleteGameInSlot(SaveSlotName, 0);
	}
	
	// Create fresh save with defaults
return LoadSave();
}