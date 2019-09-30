#include "FroxMOCSensorDevice.h"

#include "Frox/FroxDepthSensor/DepthSensorModule.h"

using namespace frox;

// Sets default values
AFroxMOCSensorDevice::AFroxMOCSensorDevice()
{}

// Called when the game starts or when spawned
void AFroxMOCSensorDevice::BeginPlay()
{
	Super::BeginPlay();

	// Add MOC
	_MocDeviceManager = IDepthSensorModule::Get().CreateDeviceManager<MOCSensorDeviceManager>();
	auto mocDevice = _MocDeviceManager->CreateDevice();
	check(mocDevice);
}

void AFroxMOCSensorDevice::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Remove MOC
	IDepthSensorModule::Get().UnRegisterDeviceManager(_MocDeviceManager.get());
	_MocDeviceManager = nullptr;

	Super::EndPlay(EndPlayReason);
}

void AFroxMOCSensorDevice::InitializeSensor()
{

}

void AFroxMOCSensorDevice::ReleaseSensor()
{

}