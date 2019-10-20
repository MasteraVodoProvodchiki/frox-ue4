#pragma once

#include "FroxArithmeticNods.h"
#include "FroxComputeFrame.h"
#include "FroxShowFrameNods.generated.h"

UCLASS(DisplayName = "ShowFrame Operation Node")
class FROXPLUGIN_API UFroxShowFrameNode : public UOpartionBasicNode
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	//~ Begin UEdGraphNode
	virtual void AllocateDefaultPins() override;
	//~ End UEdGraphNode
#endif //WITH_EDITORONLY_DATA

	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "showframe"; }
	virtual const char* GetTitle() const override { return "ShowFrame"; }
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;
	//~ End UOpartionNode

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString FrameName = FString("ShowFrame");
};

UCLASS(DisplayName = "ShowFrame with Contours Node")
class FROXPLUGIN_API UFroxShowFrameWithContoursNode : public UOpartionBasicNode
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	//~ Begin UEdGraphNode
	virtual void AllocateDefaultPins() override;
	//~ End UEdGraphNode
#endif //WITH_EDITORONLY_DATA

	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "showframe"; }
	virtual const char* GetTitle() const override { return "ShowFrameWithContours"; }
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;
	//~ End UOpartionNode

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString FrameName = FString("ShowFrameWithContours");
};