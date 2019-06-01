// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FroxEditorPlugin.h"

#include "FroxComputeFlowAssetTypeActions.h"

#include "Core.h"
#include "PropertyEditorModule.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "UnrealEd/Public/ComponentVisualizer.h"
#include "UnrealEd/Public/UnrealEdGlobals.h"
#include "Editor/UnrealEd/Classes/Editor/UnrealEdEngine.h"

#define LOCTEXT_NAMESPACE "FFroxEditorPluginModule"

void FFroxEditorPluginModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	//Custom detail views

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	EAssetTypeCategories::Type froxComputeFlowAssetCategoryBit = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("FroxAssetCategory")), LOCTEXT("FroxAssetCategory", "Frox"));

	AssetTools.RegisterAssetTypeActions(MakeShareable(new FFroxComputeFlowAssetTypeActions(froxComputeFlowAssetCategoryBit)));

	if (GUnrealEd)
	{
		/*
		GUnrealEd->RegisterComponentVisualizer(UCreaturePackMeshComponent::StaticClass()->GetFName(),
			MakeShareable(new UCreaturePackMeshVisualizer()));
		*/
	}
}

void FFroxEditorPluginModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FFroxEditorPluginModule, FroxEditorPlugin)
