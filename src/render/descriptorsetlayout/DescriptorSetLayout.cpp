#include "DescriptorSetLayout.h"
#include "../device/Device.h"

DescriptorSetLayout::DescriptorSetLayout(const Device& device) : deviceRef(device)
{
}

void DescriptorSetLayout::create()
{
	if (!ENSURE(deviceRef.getDevice() != nullptr))
		throw std::runtime_error("[DescriptorSetLayout] Device is not valid");

	const auto& device = *deviceRef.getDevice();

	// MVP 행렬 UBO 바인딩 (binding = 0, vertex shader)
	vk::DescriptorSetLayoutBinding uboLayoutBinding{
		.binding		 = 0,
		.descriptorType	 = vk::DescriptorType::eUniformBuffer,
		.descriptorCount = 1,
		.stageFlags		 = vk::ShaderStageFlagBits::eVertex,
	};

	vk::DescriptorSetLayoutCreateInfo createInfo{
		.bindingCount = 1,
		.pBindings	  = &uboLayoutBinding,
	};

	descriptorSetLayoutInst = vk::raii::DescriptorSetLayout(device, createInfo);

	std::cerr << "[DescriptorSetLayout] Created with UBO binding (binding=0, vertex)" << std::endl;
}

void DescriptorSetLayout::destroy()
{
	if (*descriptorSetLayoutInst != VK_NULL_HANDLE)
		descriptorSetLayoutInst.clear();
}
