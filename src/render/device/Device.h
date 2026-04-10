#pragma once

#include "../../../Vulkan_ToyProject_Sea.h"
#include "../IRenderClass.h"

class Device final : public IRenderClass
{
public:
	explicit Device(const class PhysicalDevice& physicalDeviceRef, const class Surface& surfaceRef);
	virtual ~Device() = default;

	Device()						 = delete;
	Device(const Device&)			 = delete;
	Device& operator=(const Device&) = delete;
	Device(Device&&)				 = delete;
	Device& operator=(Device&&)		 = delete;

public:
	const vk::raii::Device* getDevice() const
	{
		return &deviceInst;
	}

	const vk::raii::Queue* getGraphicsQueue() const
	{
		return &graphicsQueueInst;
	}

	const vk::raii::Queue* getPresentQueue() const
	{
		return &presentQueueInst;
	}

private:
	virtual void create() override;
	virtual void destroy() override;

	uint32_t findQueueFamilyIndex(const vk::raii::PhysicalDevice& physicalDevice, vk::QueueFlagBits queueFlag) const;
	uint32_t findPresentQueueFamilyIndex(const vk::raii::PhysicalDevice& physicalDevice, const vk::raii::SurfaceKHR& surface) const;

	const class PhysicalDevice& physicalDeviceRef;
	const class Surface&        surfaceRef;

	vk::raii::Device deviceInst		  = nullptr;
	vk::raii::Queue graphicsQueueInst = nullptr;
	vk::raii::Queue presentQueueInst  = nullptr;
};
