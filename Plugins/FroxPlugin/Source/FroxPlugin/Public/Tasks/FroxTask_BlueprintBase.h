#pragma once

#include "CoreMinimal.h"
#include "FroxTask_BlueprintBase.generated.h"

UCLASS(Abstract, Blueprintable)
class FROXPLUGIN_API UFroxTask_BlueprintBase : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** entry point, task will stay active until FinishExecute is called. */
	UFUNCTION(BlueprintImplementableEvent)
	void ReceiveExecute(AActor* OwnerActor);

	/** finishes task execution with Success or Fail result */
	UFUNCTION(BlueprintCallable, Category="Frox|Task")
	void FinishExecute(bool bSuccess);
};