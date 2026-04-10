#pragma once

#include "../../../Vulkan_ToyProject_Sea.h"
#include "../IRenderClass.h"

class RenderPass final : public IRenderClass
{
public:
	explicit RenderPass(const class Device& deviceRef, const class SwapChain& swapChainRef);
	virtual ~RenderPass() = default;

	RenderPass()							 = delete;
	RenderPass(const RenderPass&)			 = delete;
	RenderPass& operator=(const RenderPass&) = delete;
	RenderPass(RenderPass&&)				 = delete;
	RenderPass& operator=(RenderPass&&)		 = delete;

public:
	const vk::raii::RenderPass* getRenderPass() const
	{
		return &renderPassInst;
	}

private:
	virtual void create() override;
	virtual void destroy() override;

	const class Device& deviceRef;
	const class SwapChain& swapChainRef;

	vk::raii::RenderPass renderPassInst = nullptr;
};
