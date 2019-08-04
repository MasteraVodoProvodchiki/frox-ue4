#pragma once

#include "FroxArithmeticNods.h"
#include "FroxComputeFrame.h"
#include "FroxFindContoursNode.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EFroxFindContoursMode : uint8
{
	External 	UMETA(DisplayName = "External"),
	List 		UMETA(DisplayName = "List"),
	CComp 		UMETA(DisplayName = "CComp"),
	Tree		UMETA(DisplayName = "Tree"),
	FloodFill	UMETA(DisplayName = "FloodFill")
};

UCLASS()
class FROXPLUGIN_API UFroxFindContoursNode : public UOpartionBasicNode
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	//~ Begin UEdGraphNode
	virtual void AllocateDefaultPins() override;
	//~ End UEdGraphNode
#endif //WITH_EDITORONLY_DATA

	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "findcontours"; }
	virtual const char* GetTitle() const override { return "FindContours"; }
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;
	//~ End UOpartionNode

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EFroxFindContoursMode Mode;
};

UCLASS()
class FROXPLUGIN_API UFroxCenterOfContourNode : public UOpartionBasicNode
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	//~ Begin UEdGraphNode
	virtual void AllocateDefaultPins() override;
	//~ End UEdGraphNode
#endif //WITH_EDITORONLY_DATA

	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "centerofcontour"; }
	virtual const char* GetTitle() const override { return "CenterOfContour"; }
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;
	//~ End UOpartionNode
};

UCLASS()
class FROXPLUGIN_API UFroxRadiusOfContourNode : public UOpartionBasicNode
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	//~ Begin UEdGraphNode
	virtual void AllocateDefaultPins() override;
	//~ End UEdGraphNode
#endif //WITH_EDITORONLY_DATA

	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "radiusofcontour"; }
	virtual const char* GetTitle() const override { return "RadiusOfContour"; }
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;
	//~ End UOpartionNode
};