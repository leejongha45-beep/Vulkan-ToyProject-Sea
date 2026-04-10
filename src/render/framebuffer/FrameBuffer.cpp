#include "FrameBuffer.h"
#include "../device/Device.h"
#include "../physicaldevice/PhysicalDevice.h"
#include "../renderpass/RenderPass.h"
#include "../swapchain/SwapChain.h"

FrameBuffer::FrameBuffer(
	const Device& device,
	const PhysicalDevice& physicalDevice,
	const SwapChain& swapChain,
	const RenderPass& renderPass)
	: deviceRef(device), physicalDeviceRef(physicalDevice), swapChainRef(swapChain), renderPassRef(renderPass)
{
}

void FrameBuffer::create()
{
	if (!ENSURE(deviceRef.getDevice() != nullptr))
		throw std::runtime_error("[FrameBuffer] Device is not valid");

	const auto& device	   = *deviceRef.getDevice();
	const auto& renderPass = *renderPassRef.getRenderPass();
	const auto& imageViews = swapChainRef.getImageViews();
	auto extent			   = swapChainRef.getExtent();

	// 깊이 리소스 생성
	createDepthResources();

	// 스왑체인 이미지 수만큼 프레임버퍼 생성
	frameBufferInsts.reserve(imageViews.size());
	for (const auto& imageView : imageViews)
	{
		std::array attachments = {
			*imageView,
			*depthImageViewInst,
		};

		vk::FramebufferCreateInfo createInfo{
			.renderPass		 = *renderPass,
			.attachmentCount = static_cast<uint32_t>(attachments.size()),
			.pAttachments	 = attachments.data(),
			.width			 = extent.width,
			.height			 = extent.height,
			.layers			 = 1,
		};

		frameBufferInsts.emplace_back(device, createInfo);
	}

	std::cerr << "[FrameBuffer] Created " << frameBufferInsts.size() << " framebuffers (" << extent.width << "x"
			  << extent.height << ")" << std::endl;
}

void FrameBuffer::destroy()
{
	frameBufferInsts.clear();

	depthImageViewInst	 = nullptr;
	depthImageMemoryInst = nullptr;
	depthImageInst		 = nullptr;
}

void FrameBuffer::createDepthResources()
{
	const auto& device				 = *deviceRef.getDevice();
	auto extent						 = swapChainRef.getExtent();
	constexpr vk::Format depthFormat = vk::Format::eD32Sfloat;

	// 깊이 이미지 생성
	vk::ImageCreateInfo imageInfo{
		.imageType	   = vk::ImageType::e2D,
		.format		   = depthFormat,
		.extent		   = {extent.width, extent.height, 1},
		.mipLevels	   = 1,
		.arrayLayers   = 1,
		.samples	   = vk::SampleCountFlagBits::e1,
		.tiling		   = vk::ImageTiling::eOptimal,
		.usage		   = vk::ImageUsageFlagBits::eDepthStencilAttachment,
		.sharingMode   = vk::SharingMode::eExclusive,
		.initialLayout = vk::ImageLayout::eUndefined,
	};

	depthImageInst = vk::raii::Image(device, imageInfo);

	// 메모리 할당
	auto memRequirements = depthImageInst.getMemoryRequirements();
	auto memProperties	 = physicalDeviceRef.getPhysicalDevice()->getMemoryProperties();

	// DEVICE_LOCAL 메모리 타입 탐색
	uint32_t memoryTypeIndex = UINT32_MAX;
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((memRequirements.memoryTypeBits & (1 << i)) &&
			(memProperties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal))
		{
			memoryTypeIndex = i;
			break;
		}
	}

	if (!ENSURE(memoryTypeIndex != UINT32_MAX))
		throw std::runtime_error("[FrameBuffer] Cannot find suitable memory type for depth image");

	vk::MemoryAllocateInfo allocInfo{
		.allocationSize	 = memRequirements.size,
		.memoryTypeIndex = memoryTypeIndex,
	};

	depthImageMemoryInst = vk::raii::DeviceMemory(device, allocInfo);
	depthImageInst.bindMemory(*depthImageMemoryInst, 0);

	// 깊이 이미지 뷰 생성
	vk::ImageViewCreateInfo viewInfo{
		.image	  = *depthImageInst,
		.viewType = vk::ImageViewType::e2D,
		.format	  = depthFormat,
		.subresourceRange =
			{
				.aspectMask		= vk::ImageAspectFlagBits::eDepth,
				.baseMipLevel	= 0,
				.levelCount		= 1,
				.baseArrayLayer = 0,
				.layerCount		= 1,
			},
	};

	depthImageViewInst = vk::raii::ImageView(device, viewInfo);
}
