// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "LandscapeEditorPrivatePCH.h"
#include "LandscapeEditorCommands.h"
#include "LandscapeEdMode.h"
#include "LandscapeEditorClasses.h"


#include "LandscapeEditorDetails.h"
#include "LandscapeEditorDetailCustomizations.h"
#include "PropertyEditorModule.h"
#include "PropertyEditorDelegates.h"

#include "Editor/LevelEditor/Public/LevelEditor.h"
#include "LandscapeRender.h"

#include "Landscape/Landscape.h"

#define LOCTEXT_NAMESPACE "LandscapeEditor"

class FLandscapeEditorModule : public ILandscapeEditorModule
{
private:
	TSharedPtr<FEdModeLandscape> EdModeLandscape;
public:
	
	/**
	 * Called right after the module's DLL has been loaded and the module object has been created
	 */
	virtual void StartupModule() OVERRIDE
	{
		FLandscapeEditorCommands::Register();

		// register the editor mode
		TSharedRef<FEdModeLandscape> NewEditorMode = MakeShareable(new FEdModeLandscape);
		GEditorModeTools().RegisterMode(NewEditorMode);
		EdModeLandscape = NewEditorMode;

		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.RegisterCustomPropertyLayout("LandscapeEditorObject", FOnGetDetailCustomizationInstance::CreateStatic(&FLandscapeEditorDetails::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout("GizmoImportLayer", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FLandscapeEditorStructCustomization_FGizmoImportLayer::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout("LandscapeImportLayer", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FLandscapeEditorStructCustomization_FLandscapeImportLayer::MakeInstance));

		TSharedRef<FUICommandList> CommandList = MakeShareable(new FUICommandList);
		const FLandscapeEditorCommands& LandscapeActions = FLandscapeEditorCommands::Get();
		CommandList->MapAction(LandscapeActions.ViewModeNormal,       FExecuteAction::CreateStatic(&ChangeLandscapeViewMode, ELandscapeViewMode::Normal),       FCanExecuteAction(), FIsActionChecked::CreateStatic(&IsLandscapeViewModeSelected, ELandscapeViewMode::Normal));
		CommandList->MapAction(LandscapeActions.ViewModeLOD,          FExecuteAction::CreateStatic(&ChangeLandscapeViewMode, ELandscapeViewMode::LOD),          FCanExecuteAction(), FIsActionChecked::CreateStatic(&IsLandscapeViewModeSelected, ELandscapeViewMode::LOD));
		CommandList->MapAction(LandscapeActions.ViewModeLayerDensity, FExecuteAction::CreateStatic(&ChangeLandscapeViewMode, ELandscapeViewMode::LayerDensity), FCanExecuteAction(), FIsActionChecked::CreateStatic(&IsLandscapeViewModeSelected, ELandscapeViewMode::LayerDensity));
		CommandList->MapAction(LandscapeActions.ViewModeLayerDebug,   FExecuteAction::CreateStatic(&ChangeLandscapeViewMode, ELandscapeViewMode::DebugLayer),   FCanExecuteAction(), FIsActionChecked::CreateStatic(&IsLandscapeViewModeSelected, ELandscapeViewMode::DebugLayer));

		TSharedRef<FExtender> ViewportMenuExtender = MakeShareable(new FExtender);
		ViewportMenuExtender->AddMenuExtension("LevelViewportLandscape", EExtensionHook::First, CommandList, FMenuExtensionDelegate::CreateStatic(&ConstructLandscapeViewportMenu));
		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(ViewportMenuExtender);

		// Actor Factories
		UActorFactoryLandscape* LandscapeActorFactory = ConstructObject<UActorFactoryLandscape>(UActorFactoryLandscape::StaticClass());
		LandscapeActorFactory->NewActorClass = ALandscape::StaticClass();
		GEditor->ActorFactories.Add(LandscapeActorFactory);

		UActorFactoryLandscape* LandscapeProxyActorFactory = ConstructObject<UActorFactoryLandscape>(UActorFactoryLandscape::StaticClass());
		LandscapeProxyActorFactory->NewActorClass = ALandscapeProxy::StaticClass();
		GEditor->ActorFactories.Add(LandscapeProxyActorFactory);
	}

	/**
	 * Called before the module is unloaded, right before the module object is destroyed.
	 */
	virtual void ShutdownModule() OVERRIDE
	{
		FLandscapeEditorCommands::Unregister();

		// unregister the editor mode
		GEditorModeTools().UnregisterMode(EdModeLandscape.ToSharedRef());
		EdModeLandscape = NULL;
	}

	static void ConstructLandscapeViewportMenu(FMenuBuilder& MenuBuilder)
	{
		struct Local
		{
			static void BuildLandscapeVisualizersMenu(FMenuBuilder& InMenuBuilder)
			{
				const FLandscapeEditorCommands& LandscapeActions = FLandscapeEditorCommands::Get();

				InMenuBuilder.BeginSection("LandscapeVisualizers", LOCTEXT("LandscapeHeader", "Landscape Visualizers"));
				{
					InMenuBuilder.AddMenuEntry(LandscapeActions.ViewModeNormal, NAME_None, LOCTEXT("LandscapeViewModeNormal", "Normal"));
					InMenuBuilder.AddMenuEntry(LandscapeActions.ViewModeLOD, NAME_None, LOCTEXT("LandscapeViewModeLOD", "LOD"));
					InMenuBuilder.AddMenuEntry(LandscapeActions.ViewModeLayerDensity, NAME_None, LOCTEXT("LandscapeViewModeLayerDensity", "Layer Density"));
					InMenuBuilder.AddMenuEntry(LandscapeActions.ViewModeLayerDebug, NAME_None, LOCTEXT("LandscapeViewModeLayerDebug", "Layer Debug"));
				}
				InMenuBuilder.EndSection();
			}
		};
		MenuBuilder.AddSubMenu(LOCTEXT("LandscapeSubMenu", "Visualizers"), LOCTEXT("LandscapeSubMenu_ToolTip", "Select a Landscape visualiser"), FNewMenuDelegate::CreateStatic(&Local::BuildLandscapeVisualizersMenu));
	}

	static void ChangeLandscapeViewMode(ELandscapeViewMode::Type ViewMode)
	{
		GLandscapeViewMode = ViewMode;
	}

	static bool IsLandscapeViewModeSelected(ELandscapeViewMode::Type ViewMode)
	{
		return GLandscapeViewMode == ViewMode;
	}
};

IMPLEMENT_MODULE( FLandscapeEditorModule, LandscapeEditor );

#undef LOCTEXT_NAMESPACE
