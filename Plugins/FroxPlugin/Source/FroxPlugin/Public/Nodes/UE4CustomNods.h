#pragma once

#include "FroxNods.h"
#include "FroxComputeFrame.h"
#include "UE4CustomNods.generated.h"

UCLASS()
class FROXPLUGIN_API UUEImageReaderNode : public UOpartionNode
{
	GENERATED_BODY()

public:
	//~ Begin UEdGraphNode
#if WITH_EDITORONLY_DATA
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
#endif //WITH_EDITORONLY_DATA
	//~ End UOpartionNode

	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "UEImageReader"; }
	virtual const char* GetTitle() const override { return "UEImageReader"; }
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;
	//~ End UOpartionNode

public:
	/** width of frame */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString Path;
};