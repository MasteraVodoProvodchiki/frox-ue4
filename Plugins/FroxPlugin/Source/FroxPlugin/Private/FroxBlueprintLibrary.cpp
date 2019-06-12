#include "FroxBlueprintLibrary.h"

UFroxBlueprintLibrary::UFroxBlueprintLibrary(class FObjectInitializer const& Initializer)
	: Super(Initializer)
{}

UFroxComputeFrame* UFroxBlueprintLibrary::MakeFrame(int Width, int Height, EFroxTypeEnum Type)
{
	UFroxComputeFrame* ComputeFrame = NewObject<UFroxComputeFrame>();
	ComputeFrame->Width = Width;
	ComputeFrame->Height = Height;
	ComputeFrame->Type = Type;

	ComputeFrame->Update();

	return ComputeFrame;
}

