#include "FroxComputeFrame.h"
#include "Shared.h"
#include "FroxPlugin.h"

#include "Frox/Frox/Frox.h"

void UFroxComputeFrame::PreSave(const class ITargetPlatform* TargetPlatform)
{
	Super::PreSave(TargetPlatform);

	if (UncompressedData.Num() > 0)
	{
	}
}

void UFroxComputeFrame::PostLoad()
{
	Super::PostLoad();

	if (UncompressedData.Num() > 0)
	{
		InitializeFroxFrame();
	}
}

frox::EComputeFrameType UFroxComputeFrame::UETypeToFroxType(EFroxTypeEnum Type)
{
	switch (Type)
	{
	case EFroxTypeEnum::FTE_Bool:
		return frox::EComputeFrameType::ECFT_Bool;
	case EFroxTypeEnum::FTE_UInt8:
		return frox::EComputeFrameType::ECFT_UInt8;
	case EFroxTypeEnum::FTE_UInt16:
		return frox::EComputeFrameType::ECFT_UInt16;
	case EFroxTypeEnum::FTE_UInt32:
		return frox::EComputeFrameType::ECFT_UInt32;
	case EFroxTypeEnum::FTE_Float:
		return frox::EComputeFrameType::ECFT_Float;
	default:
		return frox::EComputeFrameType::ECFT_None;
	}
}

EFroxTypeEnum UFroxComputeFrame::FroxTypeToUEType(frox::EComputeFrameType Type)
{
	switch (Type)
	{
	case frox::ECFT_Bool:
		return EFroxTypeEnum::FTE_Bool;
	case frox::ECFT_UInt8:
		return EFroxTypeEnum::FTE_UInt8;
	case frox::ECFT_UInt16:
		return EFroxTypeEnum::FTE_UInt16;
	case frox::ECFT_UInt32:
		return EFroxTypeEnum::FTE_UInt32;
	case frox::ECFT_Float:
		return EFroxTypeEnum::FTE_Float;
	default:
		return EFroxTypeEnum::FTE_None;
	}
}

void UFroxComputeFrame::Update()
{
	frox::Frox* FroxLib = IFroxPlugin::Get().GetFrox();
	check(FroxLib != nullptr);

	frox::EComputeFrameType FroxType = UETypeToFroxType(Type);

	if (Width > 0 && Height > 0 && FroxType != frox::EComputeFrameType::ECFT_None)
	{
		_froxFrame = FroxLib->CreateComputeFrame(frox::Size{ uint32_t(Width), uint32_t(Height) }, FroxType);
	}
	else
	{
		UE_LOG(LogFrox, Error, TEXT("Invalid props"));
	}
}

void UFroxComputeFrame::FillFloat(float Value)
{
	if (!_froxFrame || _froxFrame->GetType().Type != frox::EComputeFrameType::ECFT_Float)
	{
		UE_LOG(LogFrox, Error, TEXT("Error while filing frame"));
		return;
	}

	frox::Size Size = _froxFrame->GetSize();
	for (uint32 Row = 0; Row < Size.Height; ++Row)
	{
		float* RowValues = _froxFrame->GetRowData<float>(Row);
		for (uint32 Column = 0; Column < Size.Width; ++Column)
		{
			RowValues[Column] = Value;
		}
	}
}

float UFroxComputeFrame::AtFloat(int Row, int Column) const
{
	if (!_froxFrame || _froxFrame->GetType().Type != frox::EComputeFrameType::ECFT_Float)
	{
		UE_LOG(LogFrox, Error, TEXT("Error while filing frame"));
		return 0.f;
	}

	frox::Size Size = _froxFrame->GetSize();
	if(Row < 0 || Column < 0 || uint32(Row) >= Size.Height || uint32(Column) >= Size.Width)
	{
		UE_LOG(LogFrox, Error, TEXT("Outrange At frame"));
		return 0.f;
	}

	return _froxFrame->At<float>(Row, Column);
}

void UFroxComputeFrame::SetFroxFrame(frox::ComputeFramePtr FroxFrame)
{
	check(FroxFrame != nullptr);

	_froxFrame = FroxFrame;

	frox::Size Size = _froxFrame->GetSize();
	uint32 ElementSize = _froxFrame->GetElementSize();

	Width = Size.Width;
	Height = Size.Height;
	Type = FroxTypeToUEType(_froxFrame->GetType().Type);

	// Save to UncompressedData
	uint32 NbBytes = Size.Width * Size.Height * ElementSize;
	UncompressedData.Empty(NbBytes);
	UncompressedData.AddZeroed(NbBytes);

	for (uint32 Row=0; Row <Size.Height; ++Row)
	{
		auto RowData = _froxFrame->GetRowData(Row);

		uint8* dst = UncompressedData.GetData() + Row * Size.Width * ElementSize;
		FMemory::Memcpy(dst, RowData, Size.Width * ElementSize);
	}
}

#if WITH_EDITORONLY_DATA
void UFroxComputeFrame::SetComputeFrameName(const FName &InComputeFlowFileName)
{
	ComputeFrameFileName = InComputeFlowFileName;
}

void UFroxComputeFrame::SetData(const TArray<uint8>& InUncompressedData)
{
	UncompressedData = InUncompressedData;
}
#endif

FName UFroxComputeFrame::GetComputeFlowName() const
{
	return ComputeFrameFileName;
}

void UFroxComputeFrame::InitializeFroxFrame()
{
	frox::Frox* frox = frox::FroxInstance();
	check(frox != nullptr);

	_froxFrame = frox->CreateComputeFrame(
		frox::Size{ uint32(Width), uint32(Height) },
		UETypeToFroxType(Type),
		UncompressedData.GetData()
	);
}