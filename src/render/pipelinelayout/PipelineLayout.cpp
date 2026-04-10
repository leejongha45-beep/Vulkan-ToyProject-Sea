#include "PipelineLayout.h"
#include "../descriptorsetlayout/DescriptorSetLayout.h"
#include "../device/Device.h"

PipelineLayout::PipelineLayout(const Device& device, const DescriptorSetLayout& descriptorSetLayout)
	: deviceRef(device), descriptorSetLayoutRef(descriptorSetLayout)
{
}

void PipelineLayout::create()
{
	if (!ENSURE(deviceRef.getDevice() != nullptr))
		throw std::runtime_error("[PipelineLayout] Device is not valid");

	if (!ENSURE(descriptorSetLayoutRef.getDescriptorSetLayout() != nullptr))
		throw std::runtime_error("[PipelineLayout] DescriptorSetLayout is not valid");

	const auto& device			   = *deviceRef.getDevice();
	vk::DescriptorSetLayout layout = **descriptorSetLayoutRef.getDescriptorSetLayout();

	vk::PipelineLayoutCreateInfo createInfo{
		.setLayoutCount = 1,
		.pSetLayouts	= &layout,
	};

	pipelineLayoutInst = vk::raii::PipelineLayout(device, createInfo);

	std::cerr << "[PipelineLayout] Created" << std::endl;
}

void PipelineLayout::destroy()
{
	if (*pipelineLayoutInst != VK_NULL_HANDLE)
		pipelineLayoutInst.clear();
}
