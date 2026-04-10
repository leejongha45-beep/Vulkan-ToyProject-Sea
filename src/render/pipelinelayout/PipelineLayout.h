#pragma once

#include "../../../Vulkan_ToyProject_Sea.h"
#include "../IRenderClass.h"

class PipelineLayout final : public IRenderClass
{
public:
	explicit PipelineLayout(const class Device& deviceRef, const class DescriptorSetLayout& descriptorSetLayoutRef);
	virtual ~PipelineLayout() = default;

	PipelineLayout()								 = delete;
	PipelineLayout(const PipelineLayout&)			 = delete;
	PipelineLayout& operator=(const PipelineLayout&) = delete;
	PipelineLayout(PipelineLayout&&)				 = delete;
	PipelineLayout& operator=(PipelineLayout&&)		 = delete;

public:
	const vk::raii::PipelineLayout* getPipelineLayout() const
	{
		return &pipelineLayoutInst;
	}

private:
	virtual void create() override;
	virtual void destroy() override;

	const class Device& deviceRef;
	const class DescriptorSetLayout& descriptorSetLayoutRef;

	vk::raii::PipelineLayout pipelineLayoutInst = nullptr;
};
