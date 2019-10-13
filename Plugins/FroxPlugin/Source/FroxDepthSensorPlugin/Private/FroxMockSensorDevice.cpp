#include "FroxMockSensorDevice.h"

#include "Frox/FroxDepthSensor/DepthSensorModule.h"

using namespace frox;

// Sets default values
AFroxMockSensorDevice::AFroxMockSensorDevice()
{}

// Called when the game starts or when spawned
void AFroxMockSensorDevice::BeginPlay()
{
	Super::BeginPlay();
	InitializeSensor();
}

void AFroxMockSensorDevice::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ReleaseSensor();
	Super::EndPlay(EndPlayReason);
}

void AFroxMockSensorDevice::SetDepthFrame(UFroxComputeFrame* Frame)
{
	DepthFrame = Frame;

	if (_MocDevice != nullptr)
	{
		_MocDevice->SetDepthFrame(DepthFrame != nullptr ? DepthFrame->GetFroxFrame() : nullptr);
	}
}

void AFroxMockSensorDevice::SetColorFrame(UFroxComputeFrame* Frame)
{
	ColorFrame = Frame;

	if (_MocDevice != nullptr)
	{
		_MocDevice->SetColorFrame(ColorFrame != nullptr ? ColorFrame->GetFroxFrame() : nullptr);
	}
}

void AFroxMockSensorDevice::SetInfraredFrame(UFroxComputeFrame* Frame)
{
	InfraredFrame = Frame;

	if (_MocDevice != nullptr)
	{
		_MocDevice->SetInfraredFrame(InfraredFrame != nullptr ? InfraredFrame->GetFroxFrame() : nullptr);
	}
}

void AFroxMockSensorDevice::InitializeSensor()
{
	// Add MOC
	_MockDeviceManager = IDepthSensorModule::Get().CreateDeviceManager<MockSensorDeviceManager>();
	_MocDevice = _MockDeviceManager->CreateDevice();
	check(_MocDevice);

	// Set Frames
	if (DepthFrame != nullptr)
	{	
		_MocDevice->SetDepthFrame(DepthFrame->GetFroxFrame());
	}
	
	if (ColorFrame != nullptr)
	{
		_MocDevice->SetColorFrame(ColorFrame->GetFroxFrame());
	}

	if (InfraredFrame != nullptr)
	{
		_MocDevice->SetInfraredFrame(InfraredFrame->GetFroxFrame());
	}
}

void AFroxMockSensorDevice::ReleaseSensor()
{
	// Remove MOC
	_MocDevice = nullptr;
	
	IDepthSensorModule::Get().UnRegisterDeviceManager(_MockDeviceManager.get());
	_MockDeviceManager = nullptr;
}