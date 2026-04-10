#include "PhysicalDevice.h"
#include "../instance/Instance.h"

#include <format>
#include <string>

PhysicalDevice::PhysicalDevice(const Instance& instance) : instanceRef(instance)
{
}

void PhysicalDevice::create()
{
	if (!ENSURE(instanceRef.getInstance() != nullptr))
		throw std::runtime_error("[PhysicalDevice] Instance is not valid");

	auto physicalDevices = instanceRef.getInstance()->enumeratePhysicalDevices();

	if (!ENSURE(!physicalDevices.empty()))
		throw std::runtime_error("[PhysicalDevice] No Vulkan-capable GPU found");

	// 요구 기능 정의
	struct RequiredFeature
	{
		vk::Bool32 vk::PhysicalDeviceFeatures::* featurePtr;
		const char* featureName;
	};

	constexpr std::array requiredFeatures = {
		RequiredFeature{&vk::PhysicalDeviceFeatures::samplerAnisotropy, "samplerAnisotropy"},
		RequiredFeature{&vk::PhysicalDeviceFeatures::fillModeNonSolid, "fillModeNonSolid"},
		RequiredFeature{&vk::PhysicalDeviceFeatures::wideLines, "wideLines"},
		RequiredFeature{&vk::PhysicalDeviceFeatures::tessellationShader, "tessellationShader"},
		RequiredFeature{&vk::PhysicalDeviceFeatures::geometryShader, "geometryShader"},
		RequiredFeature{&vk::PhysicalDeviceFeatures::multiDrawIndirect, "multiDrawIndirect"},
		RequiredFeature{&vk::PhysicalDeviceFeatures::shaderFloat64, "shaderFloat64"},
		RequiredFeature{&vk::PhysicalDeviceFeatures::multiViewport, "multiViewport"},
		RequiredFeature{&vk::PhysicalDeviceFeatures::depthClamp, "depthClamp"},
		RequiredFeature{&vk::PhysicalDeviceFeatures::depthBiasClamp, "depthBiasClamp"},
	};

	// 모든 물리 장치 순회 → 정보 출력 + 요구 기능 검증 → 적합한 GPU 선택
	int selectedIndex = -1;
	for (size_t i = 0; i < physicalDevices.size(); i++)
	{
		auto properties		  = physicalDevices[i].getProperties();
		auto features		  = physicalDevices[i].getFeatures();
		auto memoryProperties = physicalDevices[i].getMemoryProperties();

		std::cerr << "[PhysicalDevice] --- GPU " << i << " ---" << std::endl;
		std::cerr << "[PhysicalDevice] Name: " << properties.deviceName.data() << std::endl;
		std::cerr << "[PhysicalDevice] API Version: " << VK_VERSION_MAJOR(properties.apiVersion) << "."
				  << VK_VERSION_MINOR(properties.apiVersion) << "." << VK_VERSION_PATCH(properties.apiVersion)
				  << std::endl;
		std::cerr << "[PhysicalDevice] Driver Version: " << properties.driverVersion << std::endl;
		std::cerr << "[PhysicalDevice] Device Type: " << vk::to_string(properties.deviceType) << std::endl;

		// VRAM 크기 (DEVICE_LOCAL 힙 합산)
		vk::DeviceSize totalVram = 0;
		for (uint32_t j = 0; j < memoryProperties.memoryHeapCount; j++)
		{
			if (memoryProperties.memoryHeaps[j].flags & vk::MemoryHeapFlagBits::eDeviceLocal)
			{
				totalVram += memoryProperties.memoryHeaps[j].size;
			}
		}
		std::cerr << "[PhysicalDevice] VRAM: " << totalVram / (1024 * 1024) << " MB" << std::endl;

		// 요구 기능 검증
		std::string missingFeatures;
		for (const auto& required : requiredFeatures)
		{
			const bool isSupported = features.*(required.featurePtr);
			std::cerr << "[PhysicalDevice] " << required.featureName << ": " << (isSupported ? "YES" : "NO")
					  << std::endl;

			if (!isSupported)
			{
				if (!missingFeatures.empty())
					missingFeatures += ", ";
				missingFeatures += required.featureName;
			}
		}

		if (!missingFeatures.empty())
		{
			std::cerr << "[PhysicalDevice] GPU " << i << " REJECTED — missing: " << missingFeatures << std::endl;
			continue;
		}

		// 적합한 GPU 중 DiscreteGpu 우선, 없으면 첫 번째 적합 GPU
		if (selectedIndex < 0 || properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
		{
			selectedIndex = static_cast<int>(i);
		}
	}

	if (!ENSURE(selectedIndex >= 0))
	{
		throw std::runtime_error("[PhysicalDevice] No GPU supports all required features");
	}

	physicalDeviceInst = std::move(physicalDevices[selectedIndex]);
	std::cerr << "[PhysicalDevice] Selected GPU " << selectedIndex << ": "
			  << physicalDeviceInst.getProperties().deviceName.data() << std::endl;
}

void PhysicalDevice::destroy()
{
	if (*physicalDeviceInst != VK_NULL_HANDLE)
		physicalDeviceInst.clear();
}
