#pragma once

#include "../../../Vulkan_ToyProject_Sea.h"
#include "../IRenderClass.h"

class Device;
class PhysicalDevice;
class Surface;

class SwapChain final : public IRenderClass
{
public:
	explicit SwapChain(const Device& deviceRef, const PhysicalDevice& physicalDeviceRef, const Surface& surfaceRef, GLFWwindow* windowRef);
	virtual ~SwapChain() = default;

	SwapChain()                            = delete;
	SwapChain(const SwapChain&)            = delete;
	SwapChain& operator=(const SwapChain&) = delete;
	SwapChain(SwapChain&&)                 = delete;
	SwapChain& operator=(SwapChain&&)      = delete;

public:
	const vk::raii::SwapchainKHR* getSwapChain() const
	{
		return &swapChainInst;
	}

	vk::Format getImageFormat() const
	{
		return imageFormat;
	}

	vk::Extent2D getExtent() const
	{
		return extent;
	}

	const std::vector<vk::Image>& getImages() const
	{
		return images;
	}

	const std::vector<vk::raii::ImageView>& getImageViews() const
	{
		return imageViews;
	}

private:
	virtual void create() override;
	virtual void destroy() override;

	vk::SurfaceFormatKHR chooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const;
	vk::PresentModeKHR   choosePresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) const;
	vk::Extent2D         chooseExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const;

	void createImageViews();

	const Device&         deviceRef;
	const PhysicalDevice& physicalDeviceRef;
	const Surface&        surfaceRef;
	GLFWwindow*           windowRef = nullptr;

	vk::raii::SwapchainKHR             swapChainInst = nullptr;
	vk::Format                         imageFormat   = vk::Format::eUndefined;
	vk::Extent2D                       extent        = {};
	std::vector<vk::Image>             images;
	std::vector<vk::raii::ImageView>   imageViews;
};
