#pragma once

#include "FroxNods.h"
#include "FroxComputeFrame.h"
#include "FroxMakeNods.generated.h"

UCLASS()
class FROXPLUGIN_API UMakeFrameNode : public UOpartionNode
{
	GENERATED_BODY()

public:
	virtual const char* GetTypeName() const override { return "makeframe"; }
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;

#if WITH_EDITORONLY_DATA
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
#endif //WITH_EDITORONLY_DATA

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
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;

#if WITH_EDITORONLY_DATA
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
#endif //WITH_EDITORONLY_DATA
};
