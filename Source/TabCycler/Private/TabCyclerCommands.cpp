#include "TabCyclerCommands.h"

#define LOCTEXT_NAMESPACE "FTabCyclerCommands"

void FTabCyclerCommands::RegisterCommands()
{
    UI_COMMAND(CycleForward, "Cycle Tab Forward", "Activate the next open asset editor tab.", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(CycleBackward, "Cycle Tab Backward", "Activate the previous open asset editor tab.", EUserInterfaceActionType::Button, FInputChord());
    //UI_COMMAND(PrimeCycle, "Prime Tab Cycle", "Build the tab cycle list from currently open asset editors.", EUserInterfaceActionType::Button, FInputChord());
    //UI_COMMAND(DebugSlateTab, "Debug Active Slate Tab", "Log the active Slate window, focused widget, dock tab, and tab manager.", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE