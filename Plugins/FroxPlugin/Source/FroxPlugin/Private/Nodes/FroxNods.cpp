#include "FroxNods.h"
#include "OperationLogic.h"

#include "EdGraph/EdGraphPin.h"
#include "Frox/Frox/ComputeFlow.h"
#include "FroxTypes.h"
#include "ComputeFlowKeyType.h"

FName CategoryForType(EComputeFlowKeyType Type)
{
	if (Type == EComputeFlowKeyType::ECFKT_Frame)
	{
		return UFroxNodeBase::PC_Frame;
	}
	if (Type == EComputeFlowKeyType::ECFKT_Data)
	{
		return UFroxNodeBase::PC_Data;
	}
	if (Type != EComputeFlowKeyType::ECFKT_None)
	{
		return UFroxNodeBase::PC_Property;
	}
	return UFroxNodeBase::PC_Any;
}

FName SubCategoryForType(EComputeFlowKeyType Type)
{
	switch (Type)
	{
	case EComputeFlowKeyType::ECFKT_Bool:
		return FFroxTypeDefinition::BoolType;
	case EComputeFlowKeyType::ECFKT_Uint8:
		return FFroxTypeDefinition::UInt8Type;
	case EComputeFlowKeyType::ECFKT_Uint16:
		return FFroxTypeDefinition::UInt16Type;
	case EComputeFlowKeyType::ECFKT_Uint32:
		return FFroxTypeDefinition::UInt32Type;
	case EComputeFlowKeyType::ECFKT_Float:
		return FFroxTypeDefinition::FloatType;
	default:
		return FName();
	}
}

/// Property
void UPropertyNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName ChangedPropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UPropertyNode, PropertyName))
	{
		//
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);

	Modify();
}

#if WITH_EDITORONLY_DATA
void UPropertyNode::AllocateDefaultPins()
{
	UEdGraphPin* Output = CreatePin(EGPD_Output, UFroxNodeBase::PC_Property, TEXT("Out"));
}

FText UPropertyNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return PropertyName.IsValid() ? FText::FromName(PropertyName) : FText::FromString("Property");
}

FLinearColor UPropertyNode::GetNodeTitleColor() const
{
	return FLinearColor(108.f / 255.f, 52.f / 255.f, 131.f / 255.f);
}
#endif //WITH_EDITORONLY_DATA

UInputPropertyNode::UInputPropertyNode(const FObjectInitializer& ObjectInitializer)
	: UFroxNodeBase(ObjectInitializer)
{
	CreateNewGuid();
	LowLevelRename(*NodeGuid.ToString(), nullptr);
	bCanRenameNode = 0;
}

void UInputPropertyNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName ChangedPropertyName = PropertyChangedEvent.GetPropertyName();

	Super::PostEditChangeProperty(PropertyChangedEvent);

	Modify();
}

#if WITH_EDITORONLY_DATA
void UInputPropertyNode::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	FName ChangedPropertyName = PropertyChangedEvent.GetPropertyName();
	bool Reconstruct = false;
	bool KeysChanged = ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UInputPropertyNode, Keys);
	bool KeysChangeHandled = false;
	if (KeysChanged && (PropertyChangedEvent.ChangeType & EPropertyChangeType::ArrayRemove))
	{
		TSet<FGuid> Set;
		for (int i = 0; i < Keys.Num(); i++)
		{
			Set.Add(Keys[i].UniqueId);
		}
		for (int i = Pins.Num() - 1; i >= 0; i--)
		{
			if (!Set.Contains(Pins[i]->PinId))
			{
				Reconstruct = true;
				RemovePinAt(i, EGPD_Output);
			}
		}
		KeysChangeHandled = true;
	} 
	if (KeysChanged && (PropertyChangedEvent.ChangeType & EPropertyChangeType::ArrayClear))
	{
		Reconstruct = true;
		for (int i = Pins.Num() - 1; i >= 0; i--)
		{
			RemovePinAt(i, EGPD_Output);
		}
		KeysChangeHandled = true;
	}
	if (KeysChanged && (PropertyChangedEvent.ChangeType & EPropertyChangeType::Unspecified))
	{
		Reconstruct = true;
		TMap<FGuid, int> Map;
		for (int i = 0; i < Keys.Num(); i++)
		{
			Map.Add(Keys[i].UniqueId, i);
		}
		for (int i = Pins.Num() - 1; i >= 0; i--)
		{
			auto* Temp = Pins[i];
			int* NewIndex = Map.Find(Temp->PinId);
			Pins[i] = Pins[*NewIndex];
			Pins[*NewIndex] = Temp;
		}
		KeysChangeHandled = true;
	}
	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(FComputeFlowNodeEntry, EntryName)
		|| ChangedPropertyName == GET_MEMBER_NAME_CHECKED(FComputeFlowNodeEntry, KeyType)
		|| (KeysChanged && !KeysChangeHandled))
	{
		int Index = PropertyChangedEvent.GetArrayIndex(TEXT("Keys"));
		if (Index >= 0 && Index < Keys.Num())
		{
			auto& Entry = Keys[Index];
			TSet<FName> Set;
			for (int i = 0; i < Keys.Num(); i++)
			{
				if (i != Index) {
					Set.Add(Keys[i].EntryName);
				}
			}
			// Qick&Dirty solution, intended way to do this is implement custom collection with decent editior to allow duplication check and other features.
			bool Duplicate = false;
			Set.Add(Entry.EntryName, &Duplicate);
			// TODO Set compilation error for the node on duplicate (don't forget to track it to reset)
			if (Entry.EntryName.IsValid() && !Entry.EntryName.IsNone() && Entry.KeyType != EComputeFlowKeyType::ECFKT_None)
			{
				bool Found = false;
				for (int i = Pins.Num() - 1; i >= 0; i--)
				{
					auto* Pin = Pins[i];
					if (Pin->PinId == Entry.UniqueId)
					{
						Found = true;
						Reconstruct = true;
						if (!Duplicate)
						{
							Pin->PinName = Entry.EntryName;
							Pin->PinType.PinCategory = CategoryForType(Entry.KeyType);
							Pin->PinType.PinSubCategory = SubCategoryForType(Entry.KeyType);
						}
						else
						{
							RemovePinAt(i, EGPD_Output);
						}
						break;
					}
				}
				if (!Found && !Duplicate)
				{
					Reconstruct = true;
					UEdGraphPin* Output = CreatePin(EGPD_Output, CategoryForType(Entry.KeyType), SubCategoryForType(Entry.KeyType), Entry.EntryName);
					Output->PinId = Entry.UniqueId;
				}
			}
			else
			{
				for (int i = Pins.Num() - 1; i >= 0; i--)
				{
					if (Pins[i]->PinId == Entry.UniqueId)
					{
						Reconstruct = true;
						RemovePinAt(i, EGPD_Output);
						break;
					}
				}
			}
		}
	}

	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	Modify();
	if (Reconstruct)
	{
		ReconstructNode();
		GetGraph()->NotifyGraphChanged();
	}
}

void UInputPropertyNode::PostLoad()
{
	UFroxComputeFlowAsset* Asset = (UFroxComputeFlowAsset*)GetGraph()->GetOuter();
	if (Asset)
	{
		for (auto& key : Asset->Keys)
		{
			if (key.Direction == EComputeFlowEntryDirection::ECFED_Input)
			{
				FComputeFlowNodeEntry Entry;
				Entry.UniqueId = key.UniqueId;
				Entry.EntryDescription = key.EntryDescription;
				Entry.KeyType = key.KeyType;
				Entry.EntryName = key.EntryName;
				Keys.Add(Entry);
			}
		}
	}
	TSet<FGuid> Set;
	for (int i = Pins.Num() - 1; i >= 0; i--)
		Set.Add(Pins[i]->PinId);
	for (int i = 0; i < Keys.Num(); i++)
	{
		auto& Entry = Keys[i];
		if (Entry.EntryName.IsValid() && Entry.KeyType != EComputeFlowKeyType::ECFKT_None)
		{
			Set.Remove(Entry.UniqueId);
		}
		else
		{
			Set.Add(Entry.UniqueId);
		}
	}
	if (Set.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Keys and pins doesn't match. Recreate pins from scratch."));
		for (int i = Pins.Num() - 1; i >= 0; i--)
		{
			RemovePinAt(i, EGPD_Output);
		}

		for (int i = 0; i < Keys.Num(); i++)
		{
			auto& Entry = Keys[i];
			UEdGraphPin* Output = CreatePin(EGPD_Output, CategoryForType(Entry.KeyType), SubCategoryForType(Entry.KeyType), Entry.EntryName);
			Output->PinId = Entry.UniqueId;
		}
	}

	Super::PostLoad();
}

void UInputPropertyNode::PreSave(const class ITargetPlatform* TargetPlatform)
{
	UFroxComputeFlowAsset* Asset = (UFroxComputeFlowAsset*)GetGraph()->GetOuter();
	if (Asset)
	{
		for (int i = Asset->Keys.Num() - 1; i >= 0; i--)
		{
			if (Asset->Keys[i].Direction == EComputeFlowEntryDirection::ECFED_Input)
			{
				Asset->Keys.RemoveAt(i);
			}
		}
		for (int i = 0; i < Keys.Num(); i++)
		{
			auto& Entry = Keys[i];
			if (Entry.EntryName.IsValid() && Entry.KeyType != EComputeFlowKeyType::ECFKT_None)
			{
				FComputeFlowEntry Key;
				Key.EntryName = Entry.EntryName;
				Key.EntryDescription = Entry.EntryDescription;
				Key.KeyType = Entry.KeyType;
				Key.UniqueId = Entry.UniqueId;
				Key.Direction = EComputeFlowEntryDirection::ECFED_Input;
				Asset->Keys.Add(Key);
			}
		}
	}
}

void UInputPropertyNode::AllocateDefaultPins()
{
}

FText UInputPropertyNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("FlowInput");
}

FLinearColor UInputPropertyNode::GetNodeTitleColor() const
{
	return FLinearColor(11.f / 255.f, 102.f / 255.f, 35.f / 255.f);
}
#endif

UOutputPropertyNode::UOutputPropertyNode(const FObjectInitializer& ObjectInitializer)
	: UFroxNodeBase(ObjectInitializer)
{
	CreateNewGuid();
	LowLevelRename(*NodeGuid.ToString(), nullptr);
	bCanRenameNode = 0;
}

void UOutputPropertyNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName ChangedPropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	Super::PostEditChangeProperty(PropertyChangedEvent);

	Modify();
}

#if WITH_EDITORONLY_DATA
void UOutputPropertyNode::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	FName ChangedPropertyName = PropertyChangedEvent.GetPropertyName();
	bool Reconstruct = false;
	bool KeysChanged = ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UOutputPropertyNode, Keys);
	bool KeysChangeHandled = false;
	if (KeysChanged && (PropertyChangedEvent.ChangeType & EPropertyChangeType::ArrayRemove))
	{
		TSet<FGuid> Set;
		for (int i = 0; i < Keys.Num(); i++)
		{
			Set.Add(Keys[i].UniqueId);
		}
		for (int i = Pins.Num() - 1; i >= 0; i--)
		{
			if (!Set.Contains(Pins[i]->PinId))
			{
				Reconstruct = true;
				RemovePinAt(i, EGPD_Input);
			}
		}
		KeysChangeHandled = true;
	}
	if (KeysChanged && (PropertyChangedEvent.ChangeType & EPropertyChangeType::ArrayClear))
	{
		Reconstruct = true;
		for (int i = Pins.Num() - 1; i >= 0; i--)
		{
			RemovePinAt(i, EGPD_Input);
		}
		KeysChangeHandled = true;
	}
	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(FComputeFlowNodeEntry, EntryName)
		|| ChangedPropertyName == GET_MEMBER_NAME_CHECKED(FComputeFlowNodeEntry, KeyType)
		|| (KeysChanged && !KeysChangeHandled))
	{
		int Index = PropertyChangedEvent.GetArrayIndex(TEXT("Keys"));
		if (Index >= 0 && Index < Keys.Num())
		{
			auto& Entry = Keys[Index];
			TSet<FName> Set;
			for (int i = 0; i < Keys.Num(); i++)
			{
				if (i != Index) {
					Set.Add(Keys[i].EntryName);
				}
			}
			// Qick&Dirty solution, intended way to do this is implement custom collection with decent editior to allow duplication check and other features.
			bool Duplicate = false;
			Set.Add(Entry.EntryName, &Duplicate);
			// TODO Set compilation error for the node on duplicate (don't forget to track it to reset)
			if (Entry.EntryName.IsValid() && Entry.KeyType != EComputeFlowKeyType::ECFKT_None)
			{
				bool Found = false;
				for (int i = Pins.Num() - 1; i >= 0; i--)
				{
					auto* Pin = Pins[i];
					if (Pin->PinId == Entry.UniqueId)
					{
						Found = true;
						Reconstruct = true;
						if (!Duplicate)
						{
							Pin->PinName = Entry.EntryName;
							Pin->PinType.PinCategory = CategoryForType(Entry.KeyType);
							Pin->PinType.PinSubCategory = SubCategoryForType(Entry.KeyType);
						}
						else
						{
							RemovePinAt(i, EGPD_Input);
						}
						break;
					}
				}
				if (!Found && !Duplicate)
				{
					Reconstruct = true;
					UEdGraphPin* Output = CreatePin(EGPD_Input, CategoryForType(Entry.KeyType), SubCategoryForType(Entry.KeyType), Entry.EntryName);
					Output->PinId = Entry.UniqueId;
				}
			}
			else
			{
				for (int i = Pins.Num() - 1; i >= 0; i--)
				{
					if (Pins[i]->PinId == Entry.UniqueId)
					{
						Reconstruct = true;
						RemovePinAt(i, EGPD_Input);
						break;
					}
				}
			}
		}
	}

	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	Modify();
	if (Reconstruct)
	{
		ReconstructNode();
		GetGraph()->NotifyGraphChanged();
	}
}

void UOutputPropertyNode::PostLoad()
{
	UFroxComputeFlowAsset* Asset = (UFroxComputeFlowAsset*)GetGraph()->GetOuter();
	if (Asset)
	{
		for (auto& Key : Asset->Keys)
		{
			if (Key.Direction == EComputeFlowEntryDirection::ECFED_Output)
			{
				FComputeFlowNodeEntry Entry;
				Entry.UniqueId = Key.UniqueId;
				Entry.EntryDescription = Key.EntryDescription;
				Entry.KeyType = Key.KeyType;
				Entry.EntryName = Key.EntryName;
				Keys.Add(Entry);
			}
		}
	}
	TSet<FGuid> Set;
	for (int i = Pins.Num() - 1; i >= 0; i--)
		Set.Add(Pins[i]->PinId);
	for (int i = 0; i < Keys.Num(); i++)
	{
		auto& Entry = Keys[i];
		if (Entry.EntryName.IsValid() && Entry.KeyType != EComputeFlowKeyType::ECFKT_None)
		{
			Set.Remove(Entry.UniqueId);
		}
		else
		{
			Set.Add(Entry.UniqueId);
		}
	}
	if (Set.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Keys and pins doesn't match. Recreate pins from scratch."));
		for (int i = Pins.Num() - 1; i >= 0; i--)
		{
			RemovePinAt(i, EGPD_Input);
		}

		for (int i = 0; i < Keys.Num(); i++)
		{
			auto& Entry = Keys[i];
			UEdGraphPin* Output = CreatePin(EGPD_Input, CategoryForType(Entry.KeyType), SubCategoryForType(Entry.KeyType), Entry.EntryName);
			Output->PinId = Entry.UniqueId;
		}
	}

	Super::PostLoad();
}

void UOutputPropertyNode::PreSave(const class ITargetPlatform* TargetPlatform)
{
	UFroxComputeFlowAsset* Asset = (UFroxComputeFlowAsset*)GetGraph()->GetOuter();
	if (Asset)
	{
		for (int i = Asset->Keys.Num() - 1; i >= 0; i--)
		{
			if (Asset->Keys[i].Direction == EComputeFlowEntryDirection::ECFED_Output)
			{
				Asset->Keys.RemoveAt(i);
			}
		}
		for (int i = 0; i < Keys.Num(); i++)
		{
			auto& Entry = Keys[i];
			if (Entry.EntryName.IsValid() && Entry.KeyType != EComputeFlowKeyType::ECFKT_None)
			{
				FComputeFlowEntry Key;
				Key.EntryName = Entry.EntryName;
				Key.EntryDescription = Entry.EntryDescription;
				Key.KeyType = Entry.KeyType;
				Key.UniqueId = Entry.UniqueId;
				Key.Direction = EComputeFlowEntryDirection::ECFED_Output;
				Asset->Keys.Add(Key);
			}
		}
	}
}

void UOutputPropertyNode::AllocateDefaultPins()
{
	
}

FText UOutputPropertyNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("FlowOutput");
}

FLinearColor UOutputPropertyNode::GetNodeTitleColor() const
{
	return FLinearColor(11.f / 255.f, 102.f / 255.f, 35.f / 255.f);
}
#endif //WITH_EDITORONLY_DATA

frox::ComputeNode* UOpartionNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);
	return Flow->CreateNode(this->GetTypeName());
}

#if WITH_EDITORONLY_DATA
void UOpartionNode::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	UFroxNodeBase::PostEditChangeProperty(e);
}

FText UOpartionNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(this->GetTitle());
}
#endif //WITH_EDITORONLY_DATA