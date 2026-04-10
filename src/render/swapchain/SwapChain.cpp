#include "SwapChain.h"
#include "../device/Device.h"
#include "../physicaldevice/PhysicalDevice.h"
#include "../surface/Surface.h"

#include <algorithm>

SwapChain::SwapChain(
	const Device& device,
	const PhysicalDevice& physicalDevice,
	const Surface& surface,
	GLFWwindow* window)
	: deviceRef(device), physicalDeviceRef(physicalDevice), surfaceRef(surface), windowRef(window)
{
}

void SwapChain::create()
{
	if (!ENSURE(deviceRef.getDevice() != nullptr))
		throw std::runtime_error("[SwapChain] Device is not valid");

	if (!ENSURE(physicalDeviceRef.getPhysicalDevice() != nullptr))
		throw std::runtime_error("[SwapChain] PhysicalDevice is not valid");

	if (!ENSURE(surfaceRef.getSurface() != nullptr))
		throw std::runtime_error("[SwapChain] Surface is not valid");

	if (!ENSURE(windowRef != nullptr))
		throw std::runtime_error("[SwapChain] GLFWwindow is not valid");

	const auto& physicalDevice = *physicalDeviceRef.getPhysicalDevice();
	const auto& surface		   = *surfaceRef.getSurface();
	const auto& device		   = *deviceRef.getDevice();

	// Surface 지원 정보 조회
	auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(*surface);
	auto surfaceFormats		 = physicalDevice.getSurfaceFormatsKHR(*surface);
	auto presentModes		 = physicalDevice.getSurfacePresentModesKHR(*surface);

	if (!ENSURE(!surfaceFormats.empty()))
		throw std::runtime_error("[SwapChain] No surface formats available");

	if (!ENSURE(!presentModes.empty()))
		throw std::runtime_error("[SwapChain] No present modes available");

	// 최적 설정 선택
	auto surfaceFormat = chooseSurfaceFormat(surfaceFormats);
	auto presentMode   = choosePresentMode(presentModes);
	auto swapExtent	   = chooseExtent(surfaceCapabilities);

	// 이미지 개수 (최소 + 1, 최대 초과 방지)
	uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
	if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
	{
		imageCount = surfaceCapabilities.maxImageCount;
	}

	// 스왑체인 생성
	vk::SwapchainCreateInfoKHR createInfo{
		.surface		  = *surface,
		.minImageCount	  = imageCount,
		.imageFormat	  = surfaceFormat.format,
		.imageColorSpace  = surfaceFormat.colorSpace,
		.imageExtent	  = swapExtent,
		.imageArrayLayers = 1,
		.imageUsage		  = vk::ImageUsageFlagBits::eColorAttachment,
		.imageSharingMode = vk::SharingMode::eExclusive,
		.preTransform	  = surfaceCapabilities.currentTransform,
		.compositeAlpha	  = vk::CompositeAlphaFlagBitsKHR::eOpaque,
		.presentMode	  = presentMode,
		.clipped		  = vk::True,
	};

	swapChainInst = vk::raii::SwapchainKHR(device, createInfo);

	// 결과 저장
	imageFormat = surfaceFormat.format;
	extent		= swapExtent;
	images		= swapChainInst.getImages();

	// 이미지 뷰 생성
	createImageViews();
}

void SwapChain::destroy()
{
	imageViews.clear();
	images.clear();

	if (*swapChainInst != VK_NULL_HANDLE)
		swapChainInst.clear();
}

vk::SurfaceFormatKHR SwapChain::chooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const
{
	for (const auto& format : availableFormats)
	{
		if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			return format;
		}
	}

	return availableFormats[0];
}

vk::PresentModeKHR SwapChain::choosePresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) const
{
	for (const auto& presentMode : availablePresentModes)
	{
		if (presentMode == vk::PresentModeKHR::eMailbox)
		{
			return presentMode;
		}
	}

	return vk::PresentModeKHR::eFifo;
}

vk::Extent2D SwapChain::chooseExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const
{
	int width  = 0;
	int height = 0;
	glfwGetFramebufferSize(windowRef, &width, &height);

	return {
		std::clamp(static_cast<uint32_t>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
		std::clamp(static_cast<uint32_t>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height),
	};
}

void SwapChain::createImageViews()
{
	const auto& device = *deviceRef.getDevice();

	imageViews.reserve(images.size());
	for (const auto& image : images)
	{
		vk::ImageViewCreateInfo createInfo{
			.image	  = image,
			.viewType = vk::ImageViewType::e2D,
			.format	  = imageFormat,
			.components =
				{
					.r = vk::ComponentSwizzle::eIdentity,
					.g = vk::ComponentSwizzle::eIdentity,
					.b = vk::ComponentSwizzle::eIdentity,
					.a = vk::ComponentSwizzle::eIdentity,
				},
			.subresourceRange =
				{
					.aspectMask		= vk::ImageAspectFlagBits::eColor,
					.baseMipLevel	= 0,
					.levelCount		= 1,
					.baseArrayLayer = 0,
					.layerCount		= 1,
				},
		};

		imageViews.emplace_back(device, createInfo);
	}
}
