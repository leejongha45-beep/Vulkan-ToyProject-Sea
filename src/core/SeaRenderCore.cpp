#include "SeaRenderCore.h"
#include "../render/IRenderClass.h"
#include "../render/instance/Instance.h"
#include "../render/physicaldevice/PhysicalDevice.h"
#include "../render/surface/Surface.h"
#include "../render/device/Device.h"

SeaRenderCore::SeaRenderCore() = default;

SeaRenderCore::~SeaRenderCore()
{
	release();
}

void SeaRenderCore::initialize(GLFWwindow* window)
{
	// ===== 1. Instance (최상위) =====
	instanceInst = std::make_unique<Instance>();

	if (!ENSURE(instanceInst))
		throw std::runtime_error("[SeaRenderCore] Cannot make Instance");

	{
		IRenderClass* vulkanObject = instanceInst.get();
		vulkanObject->create();
		vulkanClasses.emplace_back(vulkanObject);
	}

	// ===== 2. Surface (Instance + GLFWwindow 의존) =====
	surfaceInst = std::make_unique<Surface>(*instanceInst, window);

	if (!ENSURE(surfaceInst))
		throw std::runtime_error("[SeaRenderCore] Cannot make Surface");

	{
		IRenderClass* vulkanObject = surfaceInst.get();
		vulkanObject->create();
		vulkanClasses.emplace_back(vulkanObject);
	}

	// ===== 3. PhysicalDevice (Instance 의존) =====
	physicalDeviceInst = std::make_unique<PhysicalDevice>(*instanceInst);

	if (!ENSURE(physicalDeviceInst))
		throw std::runtime_error("[SeaRenderCore] Cannot make PhysicalDevice");

	{
		IRenderClass* vulkanObject = physicalDeviceInst.get();
		vulkanObject->create();
		vulkanClasses.emplace_back(vulkanObject);
	}

	// ===== 4. Device (PhysicalDevice 의존) =====
	deviceInst = std::make_unique<Device>(*physicalDeviceInst, *surfaceInst);

	if (!ENSURE(deviceInst))
		throw std::runtime_error("[SeaRenderCore] Cannot make Device");

	{
		IRenderClass* vulkanObject = deviceInst.get();
		vulkanObject->create();
		vulkanClasses.emplace_back(vulkanObject);
	}
}

void SeaRenderCore::update()
{
}

void SeaRenderCore::release() noexcept
{
	for (auto vulkanclassIt = vulkanClasses.rbegin(); vulkanclassIt != vulkanClasses.rend(); ++vulkanclassIt)
	{
		auto* vulkanclass = *vulkanclassIt;

		if (ENSURE(vulkanclass))
		{
			vulkanclass->destroy();
		}
	}
}
