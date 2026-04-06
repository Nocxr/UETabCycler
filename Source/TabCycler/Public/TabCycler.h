#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Subsystems/AssetEditorSubsystem.h"

class FUICommandList;
class IAssetEditorInstance;

class FTabCyclerModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    TSharedPtr<SWidget> FindTabWell() const;

private:
    void CycleTabs(bool bForward);
    void PrimeCycleList();
    void RebuildCycleList(class UAssetEditorSubsystem* AssetEditorSubsystem);
    void DebugActiveSlateTab();

    //TSharedPtr<SWidget> FindMainTabWellFromActiveWindow() const;
    //void CollectOrderedTabs(const TSharedRef<SWidget>& TabWell, TArray<TSharedRef<SDockTab>>& OutTabs) const;

private:
    TSharedPtr<FUICommandList> PluginCommands;

    TArray<IAssetEditorInstance*> CachedCycleEditors;
    int32 CachedCycleIndex = INDEX_NONE;
};