#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveSystem/PF2eSaveFile.h"
#include "PF2eSaveSystem.generated.h"

UCLASS(Blueprintable)
class GASLEARNING_API UPF2eSaveSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public://Setting default values
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	
//
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Save System")
	FString SaveSlotName = TEXT("PF2eSaveSlot");
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Save System")
	TObjectPtr<UPF2eSaveFile> CurrentSaveFile;
	UFUNCTION(BlueprintCallable, Category = "Save System")
	bool SaveGame(UPF2eSaveFile* SaveFile);
	UFUNCTION(BlueprintCallable, Category = "Save System")
	UPF2eSaveFile* LoadSave();

	UFUNCTION(BlueprintCallable, Category = "Save System")
	bool DoesSaveExist();

	UFUNCTION(BlueprintCallable, Category = "Save System")
	UPF2eSaveFile* CreateNewSaveFile();

	UFUNCTION(BlueprintImplementableEvent, Category = "Save System")
	void SetDefaultCharacterValues(UPF2eSaveFile* SaveFile);

	UFUNCTION(BlueprintCallable, Category = "Save System")
	UPF2eSaveFile* NewGame();
};