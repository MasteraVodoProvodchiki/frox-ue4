#include "FroxComputeFlowComponent.h"
#include "FroxComputeFlowAsset.h"
#include "FroxComputeFrame.h"
#include "FroxPlugin.h"
#include "Nodes/FroxNods.h"

#include "Frox/Frox/ComputeFlow.h"
#include "Frox/Frox/ComputeFrame.h"
#include "Frox/Frox/ComputeNode.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Async.h"
#include "EdGraph/EdGraph.h"
#include "LatentActions.h"

#define LOCTEXT_NAMESPACE "FFroxPluginModule"

// FPendingLatentAction
class FFlowPerfromDelayAction : public FPendingLatentAction
{
public:
	UFroxComputeFlowComponent* CallbackComponent;
	FName ExecutionFunction;
	int32 OutputLink;
	FWeakObjectPtr CallbackTarget;
	FFroxComputeFlowPerformResult* DialogResults = nullptr;

	FFlowPerfromDelayAction(
		UFroxComputeFlowComponent* FlowComponent,
		const FLatentActionInfo& LatentInfo,
		FFroxComputeFlowPerformResult& PlayerSelection
	)
	{
		check(FlowComponent != nullptr);
		CallbackComponent = FlowComponent;
		CallbackComponent->OnPerformCompleted.AddRaw(this, &FFlowPerfromDelayAction::OnPerformCompleted);

		ExecutionFunction = LatentInfo.ExecutionFunction;
		OutputLink = LatentInfo.Linkage;
		CallbackTarget = LatentInfo.CallbackTarget;
		DialogResults = &PlayerSelection;
	
		DialogResults->Result = EFroxComputeFlowPerformResult::InProcess;
	}

	virtual ~FFlowPerfromDelayAction()
	{
		check(CallbackComponent != nullptr);
		CallbackComponent->OnPerformCompleted.RemoveAll(this);
	}

	/** BEGIN FPendingLatentAction overrides */
	virtual void UpdateOperation(FLatentResponse& Response) override
	{
		check(DialogResults);
		Response.FinishAndTriggerIf(DialogResults->Result != EFroxComputeFlowPerformResult::InProcess, ExecutionFunction, OutputLink, CallbackTarget);
	}
	/** BEGIN FPendingLatentAction overrides */

	void ReceivePerformResult(EFroxComputeFlowPerformResult Result)
	{
		check(DialogResults);
		DialogResults->Result = Result;
	}

	void OnPerformCompleted()
	{
		ReceivePerformResult(EFroxComputeFlowPerformResult::Success);
	}
};

DECLARE_DELEGATE(FComputeFlowListernerPerformedCallBack);

// IComputeFlowListerner
class FComputeFlowListerner 
	: public TSharedFromThis<FComputeFlowListerner>
	, public frox::IComputeFlowListerner
{
public:
	FComputeFlowListernerPerformedCallBack OnPerformCompleted;

	FComputeFlowListerner(FComputeFlowListernerPerformedCallBack InOnPerformCompleted)
		: OnPerformCompleted(InOnPerformCompleted)
	{}

	~FComputeFlowListerner()
	{}

	/** BEGIN IComputeFlowListerner overrides */
	virtual void OnPerformed() override
	{
		// Push Main Thread
		auto Self = AsShared();
		AsyncTask(ENamedThreads::GameThread, [Self]()
		{
			Self->OnPerformCompleted.ExecuteIfBound();
		});
	}
	/** BEGIN IComputeFlowListerner overrides */
};


UFroxComputeFlowComponent::UFroxComputeFlowComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

void UFroxComputeFlowComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (ComputeFlowAsset)
	{
		InitializeFlow(*ComputeFlowAsset);
	}
}

void UFroxComputeFlowComponent::UninitializeComponent()
{
	ReleaseFlow();
	Super::UninitializeComponent();
}

void UFroxComputeFlowComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ComputeFlow && bAutoStart)
	{
		_bRunning = true;
	}
}

void UFroxComputeFlowComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ComputeFlow)
	{
		// ComputeFlow->Stop();
	}

	Super::EndPlay(EndPlayReason);
}

void UFroxComputeFlowComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (_bRunning && ComputeFlow)
	{
		CurrentTime += DeltaTime;
		if (CurrentTime >= (1.f / PerformFrequency) && ComputeFlow->GetNumActiveTasks() == 0)
		{
			PerformFlow();
			CurrentTime = 0.f;
		}
	}
}

void UFroxComputeFlowComponent::Perform(UObject* WorldContextObject, FLatentActionInfo LatentInfo, FFroxComputeFlowPerformResult& PerformResult)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject))
	{
		FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
		if (LatentActionManager.FindExistingAction<FFlowPerfromDelayAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) == nullptr)
		{
			LatentActionManager.AddNewAction(
				LatentInfo.CallbackTarget,
				LatentInfo.UUID,
				new FFlowPerfromDelayAction(this, LatentInfo, PerformResult)
			);

			PerformFlow();
		}
	}
}

void UFroxComputeFlowComponent::Start()
{
	_bRunning = true;
}

void UFroxComputeFlowComponent::Stop()
{
	_bRunning = false;
}

bool UFroxComputeFlowComponent::IsRunning() const
{
	return _bRunning;
}

int32 UFroxComputeFlowComponent::GetValueAsInt(const FName& KeyName) const
{
	const int32* Found = IntValues.Find(KeyName);
	return Found ? *Found : 0;
}

float UFroxComputeFlowComponent::GetValueAsFloat(const FName& KeyName) const
{
	const float* Found = FloatValues.Find(KeyName);
	return Found ? *Found : 0.f;
}

bool UFroxComputeFlowComponent::GetValueAsBool(const FName& KeyName) const
{
	const bool* Found = BoolValues.Find(KeyName);
	return Found ? *Found : false;
}

UFroxComputeFrame* UFroxComputeFlowComponent::GetValueAsFrame(const FName& KeyName) const
{
	auto Found = FrameValues.Find(KeyName);
	return Found ? *Found : nullptr;
}

void UFroxComputeFlowComponent::SetValueAsInt(const FName& KeyName, int32 IntValue)
{
	IntValues.Add(KeyName, IntValue);
	OnValueChanged.Broadcast(KeyName);
}

void UFroxComputeFlowComponent::SetValueAsFloat(const FName& KeyName, float FloatValue)
{
	FloatValues.Add(KeyName, FloatValue);
	OnValueChanged.Broadcast(KeyName);
}

void UFroxComputeFlowComponent::SetValueAsBool(const FName& KeyName, bool BoolValue)
{
	BoolValues.Add(KeyName, BoolValue);
	OnValueChanged.Broadcast(KeyName);
}

void UFroxComputeFlowComponent::SetValueAsFrame(const FName& KeyName, UFroxComputeFrame* ComputeFrame)
{
	FrameValues.Add(KeyName, ComputeFrame);
	OnValueChanged.Broadcast(KeyName);
}

bool UFroxComputeFlowComponent::InitializeFlow(UFroxComputeFlowAsset& NewAsset)
{
	// Create ComputeFlow by FlowDesc
	frox::Frox* FroxLib = FFroxPluginModule::Get().GetFrox();
	check(FroxLib != nullptr);

	FComputeFlowListernerPerformedCallBack CallBack;
	CallBack.BindUObject(this, &UFroxComputeFlowComponent::OnPerformed);
	ComputeFlowListerner = TSharedPtr<FComputeFlowListerner>(new FComputeFlowListerner(CallBack));

	ComputeFlow = NewAsset.CreateFlow(ComputeFlowListerner.Get());
	check(ComputeFlow != nullptr);

	// Search IDs fro every Key
	for (const FComputeFlowEntry& entry : NewAsset.Keys)
	{
		ANSICHAR EntryName[1024];
		entry.EntryName.GetPlainANSIString(EntryName);

		if (entry.Direction == EComputeFlowEntryDirection::ECFED_Input)
		{
			int32 EntryId = ComputeFlow->FindEntryByName(EntryName);
			if (EntryId != INDEX_NONE)
			{
				InputHoles.Add(entry.EntryName, FHole{ uint32(EntryId) });
			}
			else
			{
				UE_LOG(LogFrox, Error, TEXT("Not found Entry '%s'. Check Inputs!"), EntryName);
			}
		}
		else if (entry.Direction == EComputeFlowEntryDirection::ECFED_Output)
		{
			int32 OutputId = ComputeFlow->FindOutputByName(EntryName);
			if (OutputId != INDEX_NONE)
			{
				OuputHoles.Add(entry.EntryName, FHole{ uint32(OutputId) });
			}
			else
			{
				UE_LOG(LogFrox, Error, TEXT("Not found Output '%s'. Check Outputs!"), EntryName);
			}
		}
	}

	return true;
}


void UFroxComputeFlowComponent::ReleaseFlow()
{
	if (ComputeFlow)
	{
		frox::Frox* FroxLib = FFroxPluginModule::Get().GetFrox();
		check(FroxLib != nullptr);

		FroxLib->DestroyComputeFlow(ComputeFlow);
		ComputeFlowListerner = nullptr;
	}
}

void UFroxComputeFlowComponent::FetchFlow()
{
	check(ComputeFlow != nullptr)

	// Wait
	ComputeFlow->Fetch();

	// Get Outputs
	for (const FComputeFlowEntry& entry : ComputeFlowAsset->Keys)
	{
		auto FoundOuput = OuputHoles.Find(entry.EntryName);
		if (FoundOuput)
		{
			const FHole& Ouput = *FoundOuput;
			frox::ComputeFramePtr FroxFrame = ComputeFlow->GetOutput(Ouput.EntryId);
			if (FroxFrame)
			{
				UFroxComputeFrame* ComputeFrame = GetValueAsFrame(entry.EntryName);
				if (ComputeFrame != nullptr)
				{
					ComputeFrame->SetFroxFrame(FroxFrame);
					SetValueAsFrame(entry.EntryName, ComputeFrame);
				}
				else
				{
					ComputeFrame = NewObject<UFroxComputeFrame>();
					ComputeFrame->SetFroxFrame(FroxFrame);
					SetValueAsFrame(entry.EntryName, ComputeFrame);
				}
			}
		} // End input
	} // End every key
}

void UFroxComputeFlowComponent::PerformFlow()
{
	check(ComputeFlow != nullptr)

	// Update Inputs
	for (const FComputeFlowEntry& entry : ComputeFlowAsset->Keys)
	{
		auto FoundInput = InputHoles.Find(entry.EntryName);
		if (FoundInput)
		{
			const FHole& Input = *FoundInput;
			UFroxComputeFrame* ComputeFrame = GetValueAsFrame(entry.EntryName);
			if (ComputeFrame != nullptr)
			{
				frox::ComputeFramePtr FroxFrame = ComputeFrame->GetFroxFrame();
				ComputeFlow->SetInput(Input.EntryId, FroxFrame);
			}
		} // End input
	} // End every key

	// Push Calculation
	ComputeFlow->Perform();
}

void UFroxComputeFlowComponent::OnPerformed()
{
	FetchFlow();
	OnPerformCompleted.Broadcast();
}

#undef LOCTEXT_NAMESPACE