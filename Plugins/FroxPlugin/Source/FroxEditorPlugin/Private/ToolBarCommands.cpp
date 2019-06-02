// #include "UICustomEditor.h"

#include "ToolBarCommands.h"
 
#define LOCTEXT_NAMESPACE "FFroxEditorPluginModule"

void FToolBarCommandsCommands::RegisterCommands()
{
	UI_COMMAND(ShowMessage, "Show message", "Show message", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
