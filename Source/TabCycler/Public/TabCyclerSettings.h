#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "TabCyclerSettings.generated.h"

UCLASS(Config = EditorPerProjectUserSettings, DefaultConfig)
class TABCYCLER_API UTabCyclerSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	virtual FName GetContainerName() const override { return "Editor"; }
	virtual FName GetCategoryName() const override { return "Plugins"; }
	virtual FName GetSectionName() const override { return "Tab Cycler"; }

	//UPROPERTY(Config, EditAnywhere, Category = "Cycling")
	//bool bTraverseAcrossWindows = true;
};