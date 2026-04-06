#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Styling/AppStyle.h"

class FTabCyclerCommands : public TCommands<FTabCyclerCommands>
{
public:
    FTabCyclerCommands()
        : TCommands<FTabCyclerCommands>(
            TEXT("TabCycler"),
            NSLOCTEXT("Contexts", "TabCycler", "Tab Cycler"),
            NAME_None,
            FAppStyle::GetAppStyleSetName())
    {
    }

    virtual void RegisterCommands() override;

public:
    TSharedPtr<FUICommandInfo> CycleForward;
    TSharedPtr<FUICommandInfo> CycleBackward;
    //TSharedPtr<FUICommandInfo> PrimeCycle;
    //TSharedPtr<FUICommandInfo> DebugSlateTab;
};