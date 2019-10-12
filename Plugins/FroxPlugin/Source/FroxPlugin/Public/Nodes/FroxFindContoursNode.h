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

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EFroxContourApproximationMode : uint8
{
	ChainApproxNone 		UMETA(DisplayName = "ChainApproxNone"),
	ChainApproxSimple 		UMETA(DisplayName = "ChainApproxSimple"),
	ChainApproxTC89_L1 		UMETA(DisplayName = "ChainApproxTC89_L1"),
	ChainApproxTC89_KCOS	UMETA(DisplayName = "ChainApproxTC89_KCOS")
};

UCLASS(DisplayName = "FindContours Node")
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
	EFroxFindContoursMode Mode = EFroxFindContoursMode::External;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EFroxContourApproximationMode Method = EFroxContourApproximationMode::ChainApproxSimple;
};

UCLASS(DisplayName = "CenterOfContour Node")
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

UCLASS(DisplayName = "RadiusOfContour Node")
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