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

	// Add MOC
	_MockDeviceManager = IDepthSensorModule::Get().CreateDeviceManager<MockSensorDeviceManager>();
	auto mockDevice = _MockDeviceManager->CreateDevice();
	check(mockDevice);
}

void AFroxMockSensorDevice::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Remove MOC
	IDepthSensorModule::Get().UnRegisterDeviceManager(_MockDeviceManager.get());
	_MockDeviceManager = nullptr;

	Super::EndPlay(EndPlayReason);
}

void AFroxMockSensorDevice::InitializeSensor()
{

}

void AFroxMockSensorDevice::ReleaseSensor()
{

}