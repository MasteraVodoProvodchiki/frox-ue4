#pragma once

#include "Frox/Frox/ComputeFrame.h"

#include "FroxComputeFrame.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EFroxTypeEnum : uint8
{
	FTE_None 	UMETA(DisplayName = "None"),
	FTE_Bool 	UMETA(DisplayName = "Bool"),
	FTE_UInt8 	UMETA(DisplayName = "UInt8"),
	FTE_UInt16	UMETA(DisplayName = "UInt16"),
	FTE_UInt32	UMETA(DisplayName = "UInt32"),
	FTE_Float	UMETA(DisplayName = "Float")
};

UCLASS(BlueprintType, ClassGroup = (Frox))
class FROXPLUGIN_API UFroxComputeFrame : public UObject
{
	GENERATED_BODY()

public:
	// BEGIN UObject overrides
	virtual void PostLoad() override;
	// End UObject

	UFUNCTION(BlueprintCallable, Category = "Frox")
	void Update();

	UFUNCTION(BlueprintCallable, Category = "Frox")
	void FillFloat(float Value);

	UFUNCTION(BlueprintCallable, Category = "Frox")
	float AtFloat(int Row = 0, int Column = 0) const;

	/** width of frame */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int Width = 1;

	/** height of frame */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int Height = 1;

	/** key type  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Frox)
	EFroxTypeEnum Type;

	void SetFroxFrame(frox::ComputeFramePtr FroxFrame);
	frox::ComputeFramePtr GetFroxFrame() const { return _froxFrame; }

	static frox::EComputeFrameType UETypeToFroxType(EFroxTypeEnum Type);
	static EFroxTypeEnum FroxTypeToUEType(frox::EComputeFrameType Type);

#if WITH_EDITORONLY_DATA
	void SetComputeFrameName(const FName &InComputeFlowFileName);
	void SetData(const TArray<uint8>& InUncompressedData);
#endif

	FName GetComputeFlowName() const;

protected:
	UPROPERTY()
	FName ComputeFrameFileName;

	UPROPERTY()
	TArray<uint8> UncompressedData;

private:
	void InitializeFroxFrame();

private:
	frox::ComputeFramePtr _froxFrame;
};