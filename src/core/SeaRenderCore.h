#pragma once

#include "../../Vulkan_ToyProject_Sea.h"

class SeaRenderCore final
{
public:
	SeaRenderCore();
	~SeaRenderCore();

	SeaRenderCore(const SeaRenderCore&)			   = delete;
	SeaRenderCore& operator=(const SeaRenderCore&) = delete;

	SeaRenderCore(SeaRenderCore&&)			  = delete;
	SeaRenderCore& operator=(SeaRenderCore&&) = delete;

public:
	void initialize(GLFWwindow* window);
	void update();

private:
	void release() noexcept;

private:
	std::unique_ptr<class Instance>       instanceInst;
	std::unique_ptr<class Surface>        surfaceInst;
	std::unique_ptr<class PhysicalDevice> physicalDeviceInst;
	std::unique_ptr<class Device>         deviceInst;
	std::unique_ptr<class SwapChain>      swapChainInst;
	std::unique_ptr<class RenderPass>            renderPassInst;
	std::unique_ptr<class DescriptorSetLayout>   descriptorSetLayoutInst;
	std::unique_ptr<class PipelineLayout>        pipelineLayoutInst;
	std::unique_ptr<class GraphicsPipeline>      graphicsPipelineInst;
	std::unique_ptr<class FrameBuffer>           frameBufferInst;

	std::vector<class IRenderClass*> vulkanClasses;
};