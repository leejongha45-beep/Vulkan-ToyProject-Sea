#include "Device.h"
#include "../instance/Instance.h"
#include "../physicaldevice/PhysicalDevice.h"
#include "../surface/Surface.h"

#include <format>
#include <set>
#include <string>

Device::Device(const PhysicalDevice& physicalDevice, const Surface& surface)
	: physicalDeviceRef(physicalDevice)
	, surfaceRef(surface)
{
}

void Device::create()
{
	if (!ENSURE(physicalDeviceRef.getPhysicalDevice() != nullptr))
		throw std::runtime_error("[Device] PhysicalDevice is not valid");

	const auto& physicalDevice = *physicalDeviceRef.getPhysicalDevice();

	if (!ENSURE(surfaceRef.getSurface() != nullptr))
		throw std::runtime_error("[Device] Surface is not valid");

	const auto& surface = *surfaceRef.getSurface();

	// 큐 패밀리 인덱스 탐색
	uint32_t graphicsQueueFamilyIndex = findQueueFamilyIndex(physicalDevice, vk::QueueFlagBits::eGraphics);
	uint32_t presentQueueFamilyIndex  = findPresentQueueFamilyIndex(physicalDevice, surface);

	// 중복 제거된 큐 패밀리 인덱스 목록
	std::set<uint32_t> uniqueQueueFamilyIndices = {graphicsQueueFamilyIndex, presentQueueFamilyIndex};

	// 큐 생성 정보
	constexpr float queuePriority = 1.0f;
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	queueCreateInfos.reserve(uniqueQueueFamilyIndices.size());

	for (uint32_t queueFamilyIndex : uniqueQueueFamilyIndices)
	{
		queueCreateInfos.push_back(
			vk::DeviceQueueCreateInfo{
				.queueFamilyIndex = queueFamilyIndex,
				.queueCount		  = 1,
				.pQueuePriorities = &queuePriority,
			});
	}

	// 요구 디바이스 확장
	constexpr std::array requiredDeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	// 디바이스 확장 검증
	auto extensionProperties = physicalDevice.enumerateDeviceExtensionProperties();

	std::string missingExtensions;
	for (const auto* requiredExtension : requiredDeviceExtensions)
	{
		const bool isFound = std::ranges::any_of(
			extensionProperties,
			[requiredExtension](const auto& extensionProperty)
			{
				return std::strcmp(extensionProperty.extensionName, requiredExtension) == 0;
			});

		if (!isFound)
		{
			if (!missingExtensions.empty())
				missingExtensions += ", ";
			missingExtensions += requiredExtension;
		}
	}

	if (!ENSURE(missingExtensions.empty()))
	{
		throw std::runtime_error(std::format("[Device] Missing required device extensions: {}", missingExtensions));
	}

	// 물리 장치 기능 (PhysicalDevice에서 검증 완료된 것들)
	auto enabledFeatures = physicalDevice.getFeatures();

	// 디바이스 생성
	vk::DeviceCreateInfo createInfo{
		.queueCreateInfoCount	 = static_cast<uint32_t>(queueCreateInfos.size()),
		.pQueueCreateInfos		 = queueCreateInfos.data(),
		.enabledExtensionCount	 = static_cast<uint32_t>(requiredDeviceExtensions.size()),
		.ppEnabledExtensionNames = requiredDeviceExtensions.data(),
		.pEnabledFeatures		 = &enabledFeatures,
	};

	deviceInst = vk::raii::Device(physicalDevice, createInfo);

	// 큐 핸들 획득
	graphicsQueueInst = deviceInst.getQueue(graphicsQueueFamilyIndex, 0);
	presentQueueInst  = deviceInst.getQueue(presentQueueFamilyIndex, 0);
}

void Device::destroy()
{
	graphicsQueueInst = nullptr;
	presentQueueInst  = nullptr;

	if (*deviceInst != VK_NULL_HANDLE)
		deviceInst.clear();
}

uint32_t Device::findQueueFamilyIndex(const vk::raii::PhysicalDevice& physicalDevice, vk::QueueFlagBits queueFlag) const
{
	auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

	for (uint32_t i = 0; i < queueFamilyProperties.size(); i++)
	{
		if (queueFamilyProperties[i].queueFlags & queueFlag)
		{
			return i;
		}
	}

	throw std::runtime_error(std::format("[Device] No queue family supports {}", vk::to_string(queueFlag)));
}

uint32_t Device::findPresentQueueFamilyIndex(
	const vk::raii::PhysicalDevice& physicalDevice,
	const vk::raii::SurfaceKHR&     surface) const
{
	auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

	for (uint32_t i = 0; i < queueFamilyProperties.size(); i++)
	{
		if (physicalDevice.getSurfaceSupportKHR(i, *surface))
		{
			return i;
		}
	}

	throw std::runtime_error("[Device] No queue family supports surface present");
}
