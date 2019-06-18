#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class IDetailLayoutBuilder;

/** Delegate used to retrieve current computeprops selection */
DECLARE_DELEGATE_RetVal_OneParam(int32, FOnGetSelectedComputePropsItemIndex, bool& /* bIsInherited */);

class FFroxComputePropsDataDetails : public IDetailCustomization
{
public:
	FFroxComputePropsDataDetails(FOnGetSelectedComputePropsItemIndex InOnGetSelectedBlackboardItemIndex)
		: OnGetSelectedBlackboardItemIndex(InOnGetSelectedBlackboardItemIndex)
	{}

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance(FOnGetSelectedComputePropsItemIndex InOnGetSelectedBlackboardItemIndex);

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;

private:
	/** Delegate used to retrieve current computeprops selection */
	FOnGetSelectedComputePropsItemIndex OnGetSelectedBlackboardItemIndex;
};
