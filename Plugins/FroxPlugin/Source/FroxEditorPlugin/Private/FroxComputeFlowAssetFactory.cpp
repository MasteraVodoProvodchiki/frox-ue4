#include "FroxComputeFlowAssetFactory.h"

#include "FroxComputeFlowAsset.h"
#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "UnrealEd/Public/EditorDirectories.h"
#include "Misc/FileHelper.h"
#include "Serialization/ArchiveSaveCompressedProxy.h"
#include <string>

#define LOCTEXT_NAMESPACE "FroxComputeFlowAssetFactory"

UFroxComputeFlowAssetFactory::UFroxComputeFlowAssetFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	bEditorImport = true;
	SupportedClass = UFroxComputeFlowAsset::StaticClass();

	Formats.Add(TEXT("creature_pack;CREATURE_PACK"));
}
UObject* UFroxComputeFlowAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	const FString Filter(TEXT("Frox ComputeFlow Files (*.json)|*.json"));

	UFroxComputeFlowAsset* Asset = NewObject<UFroxComputeFlowAsset>(InParent, Class, Name, Flags);
	/*
	TArray<FString> OpenFilenames;
	int32 FilterIndex = -1;
	if (FDesktopPlatformModule::Get()->OpenFileDialog(
		nullptr,
		FString(TEXT("Choose a Frox ComputeFlow file")),
		FEditorDirectories::Get().GetLastDirectory(ELastDirectory::GENERIC_IMPORT),
		TEXT(""),
		Filter,
		EFileDialogFlags::None,
		OpenFilenames,
		FilterIndex))
	{
		Asset->SetComputeFlowName(FName(*OpenFilenames[0]));
		ImportSourceFile(Asset);
	}
	*/

	return Asset;
}


bool UFroxComputeFlowAssetFactory::ImportSourceFile(UFroxComputeFlowAsset *forAsset) const
{
	FString creatureFilename = forAsset->GetComputeFlowName().ToString();
	if (forAsset == nullptr || creatureFilename.IsEmpty())
	{
		return false;
	}

	TArray<uint8> readBytes;
	if (!FFileHelper::LoadFileToArray(readBytes, *creatureFilename, 0))
	{
		return false;
	}

	/*
	forAsset->CreatureZipBinary.Reset();
	FArchiveSaveCompressedProxy Compressor =
		FArchiveSaveCompressedProxy(forAsset->CreatureZipBinary, ECompressionFlags::COMPRESS_ZLIB);

	Compressor << readBytes;
	Compressor.Flush();

	forAsset->GatherAnimationData();
	*/

	return true;
}

bool UFroxComputeFlowAssetFactory::FactoryCanImport(const FString& Filename)
{
	return true;
}

bool UFroxComputeFlowAssetFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	UFroxComputeFlowAsset* asset = Cast<UFroxComputeFlowAsset>(Obj);
	if (asset)
	{
		FString filename = asset->GetComputeFlowName().ToString();
		if (!filename.IsEmpty())
		{
			OutFilenames.Add(filename);
		}

		return true;
	}

	return false;
}

void UFroxComputeFlowAssetFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	UFroxComputeFlowAsset* asset = Cast<UFroxComputeFlowAsset>(Obj);
	if (asset && ensure(NewReimportPaths.Num() == 1))
	{
		asset->SetComputeFlowName(FName(*NewReimportPaths[0]));
	}
}

EReimportResult::Type UFroxComputeFlowAssetFactory::Reimport(UObject* Obj)
{
	if (ImportSourceFile(Cast<UFroxComputeFlowAsset>(Obj)))
	{
		// Try to find the outer package so we can dirty it up
		if (Obj->GetOuter())
		{
			Obj->GetOuter()->MarkPackageDirty();
		}
		else
		{
			Obj->MarkPackageDirty();
		}
		return EReimportResult::Succeeded;
	}
	else
	{
		return EReimportResult::Failed;
	}
}

int32 UFroxComputeFlowAssetFactory::GetPriority() const
{
	return ImportPriority;
}

#undef LOCTEXT_NAMESPACE