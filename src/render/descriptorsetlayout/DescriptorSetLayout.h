#pragma once

#include "../../../Vulkan_ToyProject_Sea.h"
#include "../IRenderClass.h"

class DescriptorSetLayout final : public IRenderClass
{
public:
	explicit DescriptorSetLayout(const class Device& deviceRef);
	virtual ~DescriptorSetLayout() = default;

	DescriptorSetLayout()									   = delete;
	DescriptorSetLayout(const DescriptorSetLayout&)			   = delete;
	DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
	DescriptorSetLayout(DescriptorSetLayout&&)				   = delete;
	DescriptorSetLayout& operator=(DescriptorSetLayout&&)	   = delete;

public:
	const vk::raii::DescriptorSetLayout* getDescriptorSetLayout() const
	{
		return &descriptorSetLayoutInst;
	}

private:
	virtual void create() override;
	virtual void destroy() override;

	const class Device& deviceRef;

	vk::raii::DescriptorSetLayout descriptorSetLayoutInst = nullptr;
};
