#pragma once

#include "FroxNods.h"
#include "FroxComputeFrame.h"
#include "FroxMakeNods.generated.h"

namespace frox {
class MakeFrameBaseComputeNode;
} // End frox

UCLASS()
class FROXPLUGIN_API UMakeFrameBaseNode : public UOpartionNode
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
#endif //WITH_EDITORONLY_DATA

protected:
	bool FillNode(frox::MakeFrameBaseComputeNode& FroxNode) const;

public:
	/** width of frame */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int Width = 1;

	/** height of frame */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int Height = 1;

	/** type of frame  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EFroxTypeEnum Type;
};

UCLASS()
class FROXPLUGIN_API UMakeFrameNode : public UMakeFrameBaseNode
{
	GENERATED_BODY()

public:
	virtual const char* GetTypeName() const override { return "makeframe"; }
	virtual const char* GetTitle() const override { return "MakeFrame"; }

	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;

public:
	/** value of frame  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString Value;
};

UCLASS()
class FROXPLUGIN_API UMakeZeroFrameNode : public UMakeFrameNode
{
	GENERATED_BODY()

public:
	virtual const char* GetTypeName() const override { return "makezeroframe"; }
	virtual const char* GetTitle() const override { return "MakeZeroFrame"; }

	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;
};

UCLASS()
class FROXPLUGIN_API UMakeNoiseFrameNode : public UMakeFrameBaseNode
{
	GENERATED_BODY()

public:
	virtual const char* GetTypeName() const override { return "makenoiseframe"; }
	virtual const char* GetTitle() const override { return "MakeNoiseFrame"; }

	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;
};
