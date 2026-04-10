#pragma once

#include "../../../Vulkan_ToyProject_Sea.h"
#include "../IRenderClass.h"

class Instance;

class PhysicalDevice final : public IRenderClass
{
public:
	explicit PhysicalDevice(const Instance& instanceRef);
	virtual ~PhysicalDevice() = default;

	PhysicalDevice()								 = delete;
	PhysicalDevice(const PhysicalDevice&)			 = delete;
	PhysicalDevice& operator=(const PhysicalDevice&) = delete;
	PhysicalDevice(PhysicalDevice&&)				 = delete;
	PhysicalDevice& operator=(PhysicalDevice&&)		 = delete;

public:
	const vk::raii::PhysicalDevice* getPhysicalDevice() const
	{
		return &physicalDeviceInst;
	}

private:
	virtual void create() override;
	virtual void destroy() override;

	const Instance& instanceRef;

	vk::raii::PhysicalDevice physicalDeviceInst = nullptr;
};
