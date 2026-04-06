#include "TabCycler.h"

#include "TabCyclerCommands.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Commands/UICommandList.h"
#include "Interfaces/IMainFrameModule.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/SWidget.h"
#include "Widgets/SWindow.h"
#include "Layout/WidgetPath.h"

void FTabCyclerModule::StartupModule()
{
	FTabCyclerCommands::Register();

	PluginCommands = MakeShared<FUICommandList>();

	PluginCommands->MapAction(
		FTabCyclerCommands::Get().CycleForward,
		FExecuteAction::CreateRaw(this, &FTabCyclerModule::CycleTabs, true));

	PluginCommands->MapAction(
		FTabCyclerCommands::Get().CycleBackward,
		FExecuteAction::CreateRaw(this, &FTabCyclerModule::CycleTabs, false));

	//PluginCommands->MapAction(
	//	FTabCyclerCommands::Get().DebugSlateTab,
	//	FExecuteAction::CreateRaw(this, &FTabCyclerModule::DebugActiveSlateTab));

	IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
	MainFrameModule.GetMainFrameCommandBindings()->Append(PluginCommands.ToSharedRef());
}

void FTabCyclerModule::ShutdownModule()
{
	PluginCommands.Reset();
	FTabCyclerCommands::Unregister();
}

TSharedPtr<SWidget> FTabCyclerModule::FindTabWell() const
{
	FSlateApplication& SlateApp = FSlateApplication::Get();

	FWidgetPath WindowPath = SlateApp.LocateWindowUnderMouse(
		SlateApp.GetCursorPos(),
		SlateApp.GetInteractiveTopLevelWindows(),
		true
	);

	TSharedPtr<SWindow> Window = WindowPath.IsValid()
		? WindowPath.GetWindow()
		: SlateApp.GetActiveTopLevelWindow();

	if (!Window.IsValid())
	{
		return nullptr;
	}

	TSharedPtr<SWidget> Found;

	TFunction<void(TSharedRef<SWidget>)> Search;
	Search = [&](TSharedRef<SWidget> Widget)
		{
			if (Found.IsValid())
			{
				return;
			}

			if (Widget->GetType() == FName("SDockingTabWell"))
			{
				Found = Widget;
				return;
			}

			FChildren* Children = Widget->GetChildren();
			if (!Children) return;

			for (int32 i = 0; i < Children->Num(); ++i)
			{
				Search(Children->GetChildAt(i));
			}
		};

	Search(Window.ToSharedRef());
	return Found;
}

void FTabCyclerModule::CycleTabs(bool bForward)
{
	TSharedPtr<SWidget> TabWell = FindTabWell();
	if (!TabWell.IsValid())
	{
		return;
	}

	TArray<TSharedRef<SDockTab>> Tabs;

	FChildren* Children = TabWell->GetChildren();
	if (!Children) return;

	for (int32 i = 0; i < Children->Num(); ++i)
	{
		TSharedRef<SWidget> Child = Children->GetChildAt(i);

		if (Child->GetType() == FName("SDockTab"))
		{
			TSharedRef<SDockTab> DockTab = StaticCastSharedRef<SDockTab>(Child);

			const FString Label = DockTab->GetTabLabel().ToString().TrimStartAndEnd();

			// skip home tab thing (havent seen any other empty name tabs?)
			if (Label.IsEmpty())
			{
				continue;
			}

			Tabs.Add(DockTab);
		}
	}

	if (Tabs.Num() <= 1)
	{
		return;
	}

	int32 ActiveIndex = 0;

	for (int32 i = 0; i < Tabs.Num(); ++i)
	{
		if (Tabs[i]->IsForeground())
		{
			ActiveIndex = i;
			break;
		}
	}

	int32 NextIndex = bForward
		? (ActiveIndex + 1) % Tabs.Num()
		: (ActiveIndex - 1 + Tabs.Num()) % Tabs.Num();

	Tabs[NextIndex]->ActivateInParent(ETabActivationCause::SetDirectly);
}

void FTabCyclerModule::DebugActiveSlateTab()
{
	FSlateApplication& SlateApp = FSlateApplication::Get();

	TSharedPtr<SWidget> FocusedWidget = SlateApp.GetKeyboardFocusedWidget();
	if (!FocusedWidget.IsValid())
	{
		FocusedWidget = SlateApp.GetUserFocusedWidget(0);
	}

	if (!FocusedWidget.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("No focused widget"));
		return;
	}

	TArray<TSharedPtr<SWidget>> DockingStacks;

	TSharedPtr<SWidget> Current = FocusedWidget;
	while (Current.IsValid())
	{
		if (Current->GetType() == FName(TEXT("SDockingTabStack")))
		{
			DockingStacks.Add(Current);
		}

		Current = Current->GetParentWidget();
	}

	if (DockingStacks.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No SDockingTabStack widgets found in parent chain"));
		return;
	}

	auto FindTabWell = [](const TSharedRef<SWidget>& RootWidget) -> TSharedPtr<SWidget>
		{
			TSharedPtr<SWidget> FoundTabWell;

			TFunction<void(TSharedRef<SWidget>)> SearchRecursive;
			SearchRecursive = [&](TSharedRef<SWidget> Widget)
				{
					if (FoundTabWell.IsValid())
					{
						return;
					}

					if (Widget->GetType() == FName(TEXT("SDockingTabWell")))
					{
						FoundTabWell = Widget;
						return;
					}

					FChildren* Children = Widget->GetChildren();
					if (Children == nullptr)
					{
						return;
					}

					for (int32 ChildIndex = 0; ChildIndex < Children->Num(); ++ChildIndex)
					{
						SearchRecursive(Children->GetChildAt(ChildIndex));
						if (FoundTabWell.IsValid())
						{
							return;
						}
					}
				};

			SearchRecursive(RootWidget);
			return FoundTabWell;
		};

	UE_LOG(LogTemp, Log, TEXT("=== ALL TAB WELLS IN PARENT CHAIN ==="));

	for (int32 StackIndex = 0; StackIndex < DockingStacks.Num(); ++StackIndex)
	{
		TSharedPtr<SWidget> TabWell = FindTabWell(DockingStacks[StackIndex].ToSharedRef());

		if (!TabWell.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("=== TAB WELL %d ==="), StackIndex);
			UE_LOG(LogTemp, Warning, TEXT("No SDockingTabWell found for this stack"));
			continue;
		}

		UE_LOG(LogTemp, Log, TEXT("=== TAB WELL %d ==="), StackIndex);

		FChildren* Tabs = TabWell->GetChildren();
		if (Tabs == nullptr || Tabs->Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Tab well has no children"));
			continue;
		}

		for (int32 TabIndex = 0; TabIndex < Tabs->Num(); ++TabIndex)
		{
			TSharedRef<SWidget> Child = Tabs->GetChildAt(TabIndex);

			if (Child->GetType() == FName(TEXT("SDockTab")))
			{
				TSharedRef<SDockTab> DockTab = StaticCastSharedRef<SDockTab>(Child);

				const FString Label = DockTab->GetTabLabel().ToString();
				const bool bIsActive = DockTab->IsForeground();

				UE_LOG(LogTemp, Log, TEXT("[%d] %s %s"),
					TabIndex,
					*Label,
					bIsActive ? TEXT("<ACTIVE>") : TEXT(""));
			}
		}
	}
}

IMPLEMENT_MODULE(FTabCyclerModule, TabCycler)