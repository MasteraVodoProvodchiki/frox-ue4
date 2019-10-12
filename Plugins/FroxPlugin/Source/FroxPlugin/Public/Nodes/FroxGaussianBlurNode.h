#pragma once

#include "FroxArithmeticNods.h"
#include "FroxComputeFrame.h"
#include "FroxGaussianBlurNode.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EGaussianBlurBorderType : uint8
{
	Constant 	UMETA(DisplayName = "Constant"),
	Replicate 	UMETA(DisplayName = "Replicate"),
	Reflect 	UMETA(DisplayName = "Reflect"),
	Wrap		UMETA(DisplayName = "Wrap"),
	Reflect101	UMETA(DisplayName = "Reflect101"),
	Transparent	UMETA(DisplayName = "Transparent"),
	Default		UMETA(DisplayName = "Default"),
	Isolated	UMETA(DisplayName = "Isolated")
};

UCLASS(DisplayName = "GaussianBlur Operation Node")
class FROXPLUGIN_API UFroxGaussianBlurNode : public UOpartionBasicNode
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	//~ Begin UEdGraphNode
	virtual void AllocateDefaultPins() override;
	//~ End UEdGraphNode
#endif //WITH_EDITORONLY_DATA

	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "gaussianblur"; }
	virtual const char* GetTitle() const override { return "GaussianBlur"; }
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;
	//~ End UOpartionNode

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FIntPoint KernelSize = FIntPoint(3, 3);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SigmaX = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SigmaY = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EGaussianBlurBorderType BorderType = EGaussianBlurBorderType::Reflect101;
};