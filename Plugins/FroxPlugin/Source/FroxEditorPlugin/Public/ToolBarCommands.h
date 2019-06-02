#pragma once

#include "SlateBasics.h"
#include "Commands.h"
#include "EditorStyle.h"

class FToolBarCommandsCommands : public TCommands<FToolBarCommandsCommands>
{
public:
	FToolBarCommandsCommands()
		: TCommands<FToolBarCommandsCommands>(TEXT("Show Message"), FText::FromString("Show Message"), NAME_None, FEditorStyle::GetStyleSetName())
	{
	}

	virtual void RegisterCommands() override;

	TSharedPtr< FUICommandInfo > ShowMessage;

};