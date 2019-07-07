#pragma once

#include "FroxArithmeticNods.h"
#include "FroxCropNods.generated.h"

UCLASS()
class FROXPLUGIN_API UCropOpartionNode : public UOpartionInput1OuputNode
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	//~ Begin UEdGraphNode
	virtual void AllocateDefaultPins() override;
	//~ End UEdGraphNode
#endif //WITH_EDITORONLY_DATA

	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "crop"; }
	virtual const char* GetTitle() const override { return "Crop"; }
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;
	//~ End UOpartionNode

	/** Offset of frame */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FIntPoint Offset;

	/** Size of frame */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FIntPoint Size = FIntPoint(1, 1);

	/** width of frame */
	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	// int X = 0;
	
	/** height of frame */
	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	// int Y = 0;

	/** width of frame */
	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	// int Width = 1;

	/** height of frame */
	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	// int Height = 1;
};