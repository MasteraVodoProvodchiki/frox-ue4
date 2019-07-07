#include "Nodes/UE4CustomNods.h"
#include "Utils.h"
#include "Shared.h"

#include "EdGraph/EdGraphPin.h"
#include "Frox/Frox/ComputeFlow.h"
#include "Frox/Frox/MakeFrameComputeNode.h"
#include "Frox/Frox/Frox.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Misc/FileHelper.h"

#define LOCTEXT_NAMESPACE "UUEImageReaderNode"

/// Base
#if WITH_EDITORONLY_DATA
void UUEImageReaderNode::AllocateDefaultPins()
{
	UEdGraphPin* Output = CreatePin(EGPD_Output, UFroxNodeBase::PC_Frame, TEXT("Out"));
}

FText UUEImageReaderNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(this->GetTitle());
}

FLinearColor UUEImageReaderNode::GetNodeTitleColor() const
{
	return FLinearColor(1.f, 0.5f, 0.f);
}
#endif //WITH_EDITORONLY_DATA

/// Make By Value
frox::ComputeNode* UUEImageReaderNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);

	auto MakeNode = Flow->CreateNode<frox::ConstFrameComputeNode>();
	check(MakeNode != nullptr);

	if (Path.IsEmpty())
	{
		UE_LOG(LogFrox, Error, TEXT("Path isn't set!"));
		return MakeNode;
	}

	
	TArray<uint8> RawFileData;
	if (FFileHelper::LoadFileToArray(RawFileData, *Path))
	{
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		// Note: PNG format.  Other formats are supported
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

		if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
		{
			// ERGBFormat ImageFormat = ImageWrapper->GetFormat();
			// TODO. Add other formats

			const TArray<uint8>* UncompressedBGRA = NULL;
			if (ImageWrapper->GetRaw(ERGBFormat::Gray, 8, UncompressedBGRA))
			{
				int32 width = ImageWrapper->GetWidth();
				int32 height = ImageWrapper->GetHeight();

				frox::Frox* frox = frox::FroxInstance();
				check(frox != nullptr);

				frox::ComputeFramePtr frame = frox->CreateComputeFrame(
					frox::Size{uint32(width), uint32(height)},
					frox::EComputeFrameType::ECFT_UInt8,
					UncompressedBGRA->GetData()
				);

				MakeNode->SetFrame(frame);
			}
		}
	} // End LoadFileToArray
	
	return MakeNode;
}


#undef LOCTEXT_NAMESPACE