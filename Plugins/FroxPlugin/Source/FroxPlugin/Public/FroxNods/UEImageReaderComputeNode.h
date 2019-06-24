#pragma once

/*
#include "Frox/Frox/ComputeNodeImpl.h"
#include "Frox/Frox/ComputeNodeFactory.h"

namespace frox {

class ComputeTask;
class UEImageReaderComputeNode : public ComputeNodeImpl
{
	FROX_COMPUTENODE(UEImageReaderComputeNode, "UEImageReader")
	using Super = ComputeNodeImpl;

public:
	UEImageReaderComputeNode(const ComputeNodeInitializer& initializer);
	virtual ~UEImageReaderComputeNode() override;

	// ComputeNode overrides
	virtual void AllocateDefaultPins() override;
	virtual bool IsValid() const override;
	virtual ComputeTask* CreateComputeTask() override;

	void SetFrame(ComputeFramePtr frame);

protected:
	// ComputeNodeImpl overrides
	virtual void OnPostInit() override;

private:
	ComputeFramePtr _frame;
	uint32_t _output;
};

} // End frox
*/