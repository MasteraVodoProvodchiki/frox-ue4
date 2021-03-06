#pragma once

#include <Factories/Factory.h>
#include <UnrealEd/Public/EditorReimportHandler.h>

#include "FroxComputeFrameAssetFactory.generated.h"


UCLASS()
class FROXEDITORPLUGIN_API UFroxComputeFrameAssetFactory : public UFactory, public FReimportHandler
{
	GENERATED_UCLASS_BODY()

	// UFactory interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	// End of UFactory interface

	// Begin FReimportHandler interface
	virtual bool FactoryCanImport(const FString& Filename) override;
	virtual bool CanReimport(UObject* Obj, TArray<FString>& OutFilenames) override;
	virtual void SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths) override;
	virtual EReimportResult::Type Reimport(UObject* Obj) override;
	virtual int32 GetPriority() const override;
	// End FReimportHandler interface

	bool ImportSourceFile(class UFroxComputeFrame *ForAsset) const;

	bool ImportImageFile(class UFroxComputeFrame *ForAsset, const FString& CreatureFilename) const;
	bool ImportGamexFile(class UFroxComputeFrame *ForAsset, const FString& CreatureFilename) const; // Import from my game engine
};