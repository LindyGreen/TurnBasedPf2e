#include "SaveSystem/PF2eSaveFile.h"

UPF2eSaveFile::UPF2eSaveFile()
{
	Chad = FS_CompleteCharacterData();
	Rogue = FS_CompleteCharacterData();
	Archer = FS_CompleteCharacterData();
	Sorcerer = FS_CompleteCharacterData();
	SaveDate = FDateTime::Now();
}