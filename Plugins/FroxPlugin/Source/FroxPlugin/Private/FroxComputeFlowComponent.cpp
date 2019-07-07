#include "FroxComputeFlowComponent.h"
#include "FroxComputeFlowAsset.h"
#include "FroxComputeFrame.h"
#include "FroxPlugin.h"
#include "Nodes/FroxNods.h"

#include "Frox/Frox/ComputeFlow.h"
#include "Frox/Frox/FlowPerformer.h"
#include "Frox/Frox/FlowData.h"
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

	virtual ~FComputeFlowListerner()
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
	, ComputeFlow(nullptr)
	, Performer(nullptr)
	, InputData(nullptr)
	, OutputData(nullptr)
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
		if (CurrentTime >= (1.f / PerformFrequency) && Performer->GetNumActiveTasks() == 0)
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
	// Create ComputeFlow
	FComputeFlowListernerPerformedCallBack CallBack;
	CallBack.BindUObject(this, &UFroxComputeFlowComponent::OnPerformed);
	ComputeFlowListerner = TSharedPtr<FComputeFlowListerner>(new FComputeFlowListerner(CallBack));

	ComputeFlow = NewAsset.CreateFlow();
	check(ComputeFlow != nullptr);

	// Create Data and Performer
	frox::Frox* FroxLib = FFroxPluginModule::Get().GetFrox();
	check(FroxLib != nullptr);

	Performer = FroxLib->CreateFlowPerformer(ComputeFlowListerner.Get());
	check(Performer != nullptr);

	InputData = FroxLib->CreateFlowData();
	check(InputData != nullptr);

	OutputData = FroxLib->CreateFlowData();
	check(OutputData != nullptr);

	// Search IDs fro every Key
	for (const FComputeFlowEntry& Entry : NewAsset.Keys)
	{
		ANSICHAR EntryName[1024];
		Entry.EntryName.GetPlainANSIString(EntryName);

		if (Entry.Direction == EComputeFlowEntryDirection::ECFED_Input)
		{
			int32 EntryId = ComputeFlow->FindEntryByName(EntryName);
			if (EntryId == INDEX_NONE)
			{
				UE_LOG(LogFrox, Error, TEXT("Not found Entry '%s'. Check Inputs!"), EntryName);
			}
		}
		else if (Entry.Direction == EComputeFlowEntryDirection::ECFED_Output)
		{
			int32 OutputId = ComputeFlow->FindOutputByName(EntryName);
			if (OutputId == INDEX_NONE)
			{
				UE_LOG(LogFrox, Error, TEXT("Not found Output '%s'. Check Outputs!"), EntryName);
			}
		}
	}

	return true;
}


void UFroxComputeFlowComponent::ReleaseFlow()
{
	if (OutputData)
	{
		OutputData->Release();
		OutputData = nullptr;
	}

	if (InputData)
	{
		InputData->Release();
		InputData = nullptr;
	}

	if (Performer)
	{
		Performer->Release();
		Performer = nullptr;
		ComputeFlowListerner = nullptr;
	}

	if (ComputeFlow)
	{
		ComputeFlow->Release();
		ComputeFlow = nullptr;
	}
}

void UFroxComputeFlowComponent::FetchFlow()
{
	check(ComputeFlow != nullptr);
	check(Performer != nullptr);
	check(OutputData != nullptr);

	// Wait
	FlowPerformer->Fetch(ComputeFlow, FlowOutputData);

	// Get Outputs
	for (const FComputeFlowEntry& Entry : ComputeFlowAsset->Keys)
	{
		ANSICHAR EntryName[1024];
		Entry.EntryName.GetPlainANSIString(EntryName);

		auto FoundOuput = OuputHoles.Find(Entry.EntryName);
		if (FoundOuput)
		{
			const FHole& Ouput = *FoundOuput;
			frox::ComputeFramePtr FroxFrame = FlowOutputData->GetFrame(EntryName);
			if (FroxFrame)
			{
				UFroxComputeFrame* ComputeFrame = GetValueAsFrame(Entry.EntryName);
				if (ComputeFrame != nullptr)
				{
					ComputeFrame->SetFroxFrame(FroxFrame);
					SetValueAsFrame(Entry.EntryName, ComputeFrame);
				}
				else
				{
					ComputeFrame = NewObject<UFroxComputeFrame>();
					ComputeFrame->SetFroxFrame(FroxFrame);
					SetValueAsFrame(Entry.EntryName, ComputeFrame);
				}
			}
		} // End input
	} // End every key
}

void UFroxComputeFlowComponent::PerformFlow()
{
	check(ComputeFlow != nullptr);
	check(Performer != nullptr);
	check(InputData != nullptr);

	// Update Inputs
	for (const FComputeFlowEntry& Entry : ComputeFlowAsset->Keys)
	{
		ANSICHAR EntryName[1024];
		Entry.EntryName.GetPlainANSIString(EntryName);

		auto FoundInput = InputHoles.Find(Entry.EntryName);
		if (FoundInput)
		{
			const FHole& Input = *FoundInput;
			UFroxComputeFrame* ComputeFrame = GetValueAsFrame(Entry.EntryName);
			if (ComputeFrame != nullptr)
			{
				frox::ComputeFramePtr FroxFrame = ComputeFrame->GetFroxFrame();
				FlowInputData->SetFrame(EntryName, FroxFrame);
			}
		} // End input
	} // End every key

	// Push Calculation
	Performer->Perform(ComputeFlow, InputData);
}

void UFroxComputeFlowComponent::OnPerformed()
{
	if (ComputeFlow == nullptr)
	{
		UE_LOG(LogFrox, Error, TEXT("OnPerformed: ComputeFlow == null"));
		return;
	}

	FetchFlow();
	OnPerformCompleted.Broadcast();
}

#undef LOCTEXT_NAMESPACE