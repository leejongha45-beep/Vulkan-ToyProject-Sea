#include "Instance.h"

#include <format>
#include <ranges>
#include <string>

void Instance::create()
{
	vk::ApplicationInfo appInfo{
		.pApplicationName	= "Sea",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName		= "SeaEngine",
		.apiVersion			= VK_API_VERSION_1_4};

	// 확장 검사
	auto extensionProperties	= contextInst.enumerateInstanceExtensionProperties();
	auto requiredExtensions		= getRequiredExtensions();
	bool isAllUseableExtensions = std::ranges::all_of(
		requiredExtensions,
		[&extensionProperties](const auto& requiredExtension)
		{
			auto useableExtensionIt = std::ranges::find_if(
				extensionProperties,
				[requiredExtension](const auto& extensionProperty)
				{
					return std::strcmp(extensionProperty.extensionName, requiredExtension) == 0;
				});

			return useableExtensionIt != extensionProperties.end();
		});

	if (!ENSURE(isAllUseableExtensions))
	{
		std::string missingExtensions;
		for (const auto* requiredExtension : requiredExtensions)
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

		throw std::runtime_error(std::format("[Instance] Cannot find required extensions: {}", missingExtensions));
	}

	// 디버깅 레이어 검사
	constexpr std::array requiredLayers = {"VK_LAYER_KHRONOS_validation"};
	if constexpr (enableValidationLayers)
	{
		auto layerProperties	= contextInst.enumerateInstanceLayerProperties();
		bool isAllUseableLayers = std::ranges::all_of(
			requiredLayers,
			[&layerProperties](const auto& requiredLayer)
			{
				return std::ranges::any_of(
					layerProperties,
					[requiredLayer](const auto& layerProperty)
					{
						return std::strcmp(requiredLayer, layerProperty.layerName) == 0;
					});
			});

		if (!ENSURE(isAllUseableLayers))
		{
			std::string missingLayers;
			for (const auto* requiredLayer : requiredLayers)
			{
				const bool isFound = std::ranges::any_of(
					layerProperties,
					[requiredLayer](const auto& layerProperty)
					{
						return std::strcmp(layerProperty.layerName, requiredLayer) == 0;
					});

				if (!isFound)
				{
					if (!missingLayers.empty())
						missingLayers += ", ";
					missingLayers += requiredLayer;
				}
			}

			throw std::runtime_error(std::format("[Instance] Cannot find required layers: {}", missingLayers));
		}
	}

	// 생성
	vk::InstanceCreateInfo createInfo{
		.pApplicationInfo		 = &appInfo,
		.enabledLayerCount		 = 0,
		.ppEnabledLayerNames	 = nullptr,
		.enabledExtensionCount	 = static_cast<uint32_t>(requiredExtensions.size()),
		.ppEnabledExtensionNames = requiredExtensions.data()};

	if constexpr (enableValidationLayers)
	{
		createInfo.enabledLayerCount   = static_cast<uint32_t>(requiredLayers.size());
		createInfo.ppEnabledLayerNames = requiredLayers.data();
	}

	instanceInst = vk::raii::Instance(contextInst, createInfo);

	// 디버그 메신저 생성
	if constexpr (enableValidationLayers)
	{
		createDebugMessenger();
	}
}

void Instance::destroy()
{
	if constexpr (enableValidationLayers)
	{
		if (*debugMessengerInst != VK_NULL_HANDLE)
			debugMessengerInst.clear();
	}

	if (*instanceInst != VK_NULL_HANDLE)
		instanceInst.clear();
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL Instance::debugCallback(
	vk::DebugUtilsMessageSeverityFlagBitsEXT       messageSeverity,
	vk::DebugUtilsMessageTypeFlagsEXT              messageTypes,
	const vk::DebugUtilsMessengerCallbackDataEXT*  pCallbackData,
	void*                                          pUserData)
{
	if (messageSeverity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
	{
		std::cerr << "[Vulkan Validation] " << pCallbackData->pMessage << std::endl;
	}

	return vk::False;
}

void Instance::createDebugMessenger()
{
	vk::DebugUtilsMessengerCreateInfoEXT createInfo{
		.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
						 | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
						 | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
		.messageType     = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
					     | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
					     | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
		.pfnUserCallback = &debugCallback,
		.pUserData       = nullptr,
	};

	debugMessengerInst = vk::raii::DebugUtilsMessengerEXT(instanceInst, createInfo);
}

std::vector<const char*> Instance::getRequiredExtensions() const
{
	uint32_t glfwExtensionCount = 0;
	auto glfwExtensions			= glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if constexpr (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}
