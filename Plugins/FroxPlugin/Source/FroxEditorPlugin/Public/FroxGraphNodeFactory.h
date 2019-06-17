#pragma once

#include "EdGraphUtilities.h"

class FFroxGraphNodeFactory : public FGraphPanelNodeFactory
{
public:
	FFroxGraphNodeFactory();

private:
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override;
};
