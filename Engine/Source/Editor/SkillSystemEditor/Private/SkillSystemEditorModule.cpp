// Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.

#include "SkillSystemEditorModulePrivatePCH.h"
#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"
#include "AttributeDetails.h"
#include "AttributeSet.h"

class FSkillSystemEditorModule : public ISkillSystemEditorModule
{
	// Begin IModuleInterface
	virtual void StartupModule() OVERRIDE;
	virtual void ShutdownModule() OVERRIDE;
	// End IModuleInterface
};

IMPLEMENT_MODULE( FSkillSystemEditorModule, SkillSystemEditor )

void FSkillSystemEditorModule::StartupModule()
{
	// Register the details customizer
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout( "GameplayAttribute", FOnGetPropertyTypeCustomizationInstance::CreateStatic( &FAttributePropertyDetails::MakeInstance ) );
	PropertyModule.RegisterCustomPropertyTypeLayout( "ScalableFloat", FOnGetPropertyTypeCustomizationInstance::CreateStatic( &FScalableFloatDetails::MakeInstance ) );

	PropertyModule.RegisterCustomPropertyLayout( "AttributeSet", FOnGetDetailCustomizationInstance::CreateStatic( &FAttributeDetails::MakeInstance ) );
}

void FSkillSystemEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}
