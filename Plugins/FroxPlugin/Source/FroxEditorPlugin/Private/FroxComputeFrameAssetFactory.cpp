#include "FroxComputeFrameAssetFactory.h"
#include "FroxComputeFrame.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Misc/FileHelper.h"

//#include "IDesktopPlatform.h"
//#include "DesktopPlatformModule.h"

#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "UnrealEd/Public/EditorDirectories.h"

#define LOCTEXT_NAMESPACE "FroxComputeFrameAssetFactory"

UFroxComputeFrameAssetFactory::UFroxComputeFrameAssetFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	bEditorImport = true;
	SupportedClass = UFroxComputeFrame::StaticClass();

	Formats.Add(TEXT("creature_pack;CREATURE_PACK"));
}
UObject* UFroxComputeFrameAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	const FString Filter(TEXT("Frox ComputeFrame Files (*.png)|*.png"));

	UFroxComputeFrame* ComputeFrame = NewObject<UFroxComputeFrame>(InParent, Class, Name, Flags);

	TArray<FString> OpenFilenames;
	int32 FilterIndex = -1;
	if (FDesktopPlatformModule::Get()->OpenFileDialog(
		nullptr,
		FString(TEXT("Choose a Frox ComputeFrame file")),
		FEditorDirectories::Get().GetLastDirectory(ELastDirectory::GENERIC_IMPORT),
		TEXT(""),
		Filter,
		EFileDialogFlags::None,
		OpenFilenames,
		FilterIndex))
	{
		ComputeFrame->SetComputeFrameName(FName(*OpenFilenames[0]));
		ImportSourceFile(ComputeFrame);
	}

	return ComputeFrame;
}


bool UFroxComputeFrameAssetFactory::ImportSourceFile(UFroxComputeFrame *ForComputeFrame) const
{
	FString CreatureFilename = ForComputeFrame->GetComputeFlowName().ToString();
	if (ForComputeFrame == nullptr || CreatureFilename.IsEmpty())
	{
		return false;
	}
	
	TArray<uint8> RawFileData;
	if (FFileHelper::LoadFileToArray(RawFileData, *CreatureFilename))
	{
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		// Note: PNG format.  Other formats are supported
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

		if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
		{
			// ERGBFormat ImageFormat = ImageWrapper->GetFormat();
			// TODO. Add other formats

			const TArray<uint8>* UncompressedData = nullptr;
			if (ImageWrapper->GetRaw(ERGBFormat::Gray, 8, UncompressedData))
			{
				int32 Width = ImageWrapper->GetWidth();
				int32 Height = ImageWrapper->GetHeight();

				ForComputeFrame->Width = Width;
				ForComputeFrame->Height = Height;
				ForComputeFrame->Type = EFroxTypeEnum::FTE_UInt8;
				ForComputeFrame->SetData(*UncompressedData);
			}
		}
	} // End LoadFileToArray

	return true;
}

bool UFroxComputeFrameAssetFactory::FactoryCanImport(const FString& Filename)
{
	return true;
}

bool UFroxComputeFrameAssetFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	UFroxComputeFrame* asset = Cast<UFroxComputeFrame>(Obj);
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

void UFroxComputeFrameAssetFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	UFroxComputeFrame* asset = Cast<UFroxComputeFrame>(Obj);
	if (asset && ensure(NewReimportPaths.Num() == 1))
	{
		asset->SetComputeFrameName(FName(*NewReimportPaths[0]));
	}
}

EReimportResult::Type UFroxComputeFrameAssetFactory::Reimport(UObject* Obj)
{
	if (ImportSourceFile(Cast<UFroxComputeFrame>(Obj)))
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

int32 UFroxComputeFrameAssetFactory::GetPriority() const
{
	return ImportPriority;
}

#undef LOCTEXT_NAMESPACE