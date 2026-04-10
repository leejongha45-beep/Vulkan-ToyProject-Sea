#pragma once

#include "../../../Vulkan_ToyProject_Sea.h"
#include "../IRenderClass.h"

class FrameBuffer final : public IRenderClass
{
public:
	explicit FrameBuffer(
		const class Device& deviceRef,
		const class PhysicalDevice& physicalDeviceRef,
		const class SwapChain& swapChainRef,
		const class RenderPass& renderPassRef);
	virtual ~FrameBuffer() = default;

	FrameBuffer()							   = delete;
	FrameBuffer(const FrameBuffer&)			   = delete;
	FrameBuffer& operator=(const FrameBuffer&) = delete;
	FrameBuffer(FrameBuffer&&)				   = delete;
	FrameBuffer& operator=(FrameBuffer&&)	   = delete;

public:
	const std::vector<vk::raii::Framebuffer>& getFrameBuffers() const
	{
		return frameBufferInsts;
	}

private:
	virtual void create() override;
	virtual void destroy() override;

	void createDepthResources();

	const class Device& deviceRef;
	const class PhysicalDevice& physicalDeviceRef;
	const class SwapChain& swapChainRef;
	const class RenderPass& renderPassRef;

	vk::raii::Image depthImageInst				= nullptr;
	vk::raii::DeviceMemory depthImageMemoryInst = nullptr;
	vk::raii::ImageView depthImageViewInst		= nullptr;

	std::vector<vk::raii::Framebuffer> frameBufferInsts;
};
