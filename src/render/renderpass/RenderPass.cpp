#include "RenderPass.h"
#include "../device/Device.h"
#include "../swapchain/SwapChain.h"

RenderPass::RenderPass(const Device& device, const SwapChain& swapChain) : deviceRef(device), swapChainRef(swapChain)
{
}

void RenderPass::create()
{
	if (!ENSURE(deviceRef.getDevice() != nullptr))
		throw std::runtime_error("[RenderPass] Device is not valid");

	const auto& device = *deviceRef.getDevice();

	// Color attachment (스왑체인 이미지)
	vk::AttachmentDescription colorAttachment{
		.format			= swapChainRef.getImageFormat(),
		.samples		= vk::SampleCountFlagBits::e1,
		.loadOp			= vk::AttachmentLoadOp::eClear,
		.storeOp		= vk::AttachmentStoreOp::eStore,
		.stencilLoadOp	= vk::AttachmentLoadOp::eDontCare,
		.stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
		.initialLayout	= vk::ImageLayout::eUndefined,
		.finalLayout	= vk::ImageLayout::ePresentSrcKHR,
	};

	vk::AttachmentReference colorAttachmentRef{
		.attachment = 0,
		.layout		= vk::ImageLayout::eColorAttachmentOptimal,
	};

	// Depth attachment
	vk::AttachmentDescription depthAttachment{
		.format			= vk::Format::eD32Sfloat,
		.samples		= vk::SampleCountFlagBits::e1,
		.loadOp			= vk::AttachmentLoadOp::eClear,
		.storeOp		= vk::AttachmentStoreOp::eDontCare,
		.stencilLoadOp	= vk::AttachmentLoadOp::eDontCare,
		.stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
		.initialLayout	= vk::ImageLayout::eUndefined,
		.finalLayout	= vk::ImageLayout::eDepthStencilAttachmentOptimal,
	};

	vk::AttachmentReference depthAttachmentRef{
		.attachment = 1,
		.layout		= vk::ImageLayout::eDepthStencilAttachmentOptimal,
	};

	// Subpass
	vk::SubpassDescription subpass{
		.pipelineBindPoint		 = vk::PipelineBindPoint::eGraphics,
		.colorAttachmentCount	 = 1,
		.pColorAttachments		 = &colorAttachmentRef,
		.pDepthStencilAttachment = &depthAttachmentRef,
	};

	// Subpass dependency (외부 → subpass 0 전환 동기화)
	vk::SubpassDependency dependency{
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask =
			vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
		.dstStageMask =
			vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
		.srcAccessMask = {},
		.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite,
	};

	// RenderPass 생성
	std::array attachments = {colorAttachment, depthAttachment};

	vk::RenderPassCreateInfo createInfo{
		.attachmentCount = static_cast<uint32_t>(attachments.size()),
		.pAttachments	 = attachments.data(),
		.subpassCount	 = 1,
		.pSubpasses		 = &subpass,
		.dependencyCount = 1,
		.pDependencies	 = &dependency,
	};

	renderPassInst = vk::raii::RenderPass(device, createInfo);
}

void RenderPass::destroy()
{
	if (*renderPassInst != VK_NULL_HANDLE)
		renderPassInst.clear();
}
