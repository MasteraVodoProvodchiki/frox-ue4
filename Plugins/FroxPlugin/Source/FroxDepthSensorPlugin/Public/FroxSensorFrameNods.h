#pragma once

#include "FroxNods.h"
#include "FroxSensorFrameNods.generated.h"

UCLASS()
class FROXDEPTHSENSORPLUGIN_API UFroxSensorFrameNode : public UOpartionNode
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	//~ Begin UEdGraphNode
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	//~ End UEdGraphNode
#endif //WITH_EDITORONLY_DATA
};

UCLASS()
class FROXDEPTHSENSORPLUGIN_API UFroxSensorDepthFrameNode : public UFroxSensorFrameNode
{
	GENERATED_BODY()

public:
	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "sensordepthframe"; }
	virtual const char* GetTitle() const override { return "SensorDepthFrame"; }
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;
	//~ End UOpartionNode
};

UCLASS()
class FROXDEPTHSENSORPLUGIN_API UFroxSensorColorFrameNode : public UFroxSensorFrameNode
{
	GENERATED_BODY()

public:
	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "sensorcolorframe"; }
	virtual const char* GetTitle() const override { return "SensorColorFrame"; }
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;
	//~ End UOpartionNode
};

UCLASS()
class FROXDEPTHSENSORPLUGIN_API UFroxSensorInfraredFrameNode : public UFroxSensorFrameNode
{
	GENERATED_BODY()

public:
	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "sensorinfraredframe"; }
	virtual const char* GetTitle() const override { return "SensorInfraredFrame"; }
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;
	//~ End UOpartionNode
};