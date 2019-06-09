// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FroxComputeFlowCommands.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "FroxComputeFlowCommands"


FFroxComputeFlowPropsCommands::FFroxComputeFlowPropsCommands()
	: TCommands<FFroxComputeFlowPropsCommands>("FroxEditor.Props", LOCTEXT("ComputeProps", "ComputeProps"), NAME_None, FEditorStyle::GetStyleSetName())
{
}

void FFroxComputeFlowPropsCommands::RegisterCommands()
{
	UI_COMMAND(DeleteEntry, "Delete", "Delete this compute flow entry", EUserInterfaceActionType::Button, FInputChord(EKeys::Platform_Delete));
}

#undef LOCTEXT_NAMESPACE
