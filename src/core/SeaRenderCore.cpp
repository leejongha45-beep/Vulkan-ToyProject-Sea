#include "SeaRenderCore.h"
#include "../render/IRenderClass.h"
#include "../render/instance/Instance.h"
#include "../render/physicaldevice/PhysicalDevice.h"
#include "../render/surface/Surface.h"
#include "../render/device/Device.h"
#include "../render/swapchain/SwapChain.h"
#include "../render/renderpass/RenderPass.h"
#include "../render/descriptorsetlayout/DescriptorSetLayout.h"
#include "../render/pipelinelayout/PipelineLayout.h"
#include "../render/graphicspipeline/GraphicsPipeline.h"
#include "../render/framebuffer/FrameBuffer.h"

SeaRenderCore::SeaRenderCore() = default;

SeaRenderCore::~SeaRenderCore()
{
	release();
}

void SeaRenderCore::initialize(GLFWwindow* window)
{
	// ===== 1. Instance (최상위) =====
	instanceInst = std::make_unique<Instance>();

	if (!ENSURE(instanceInst))
		throw std::runtime_error("[SeaRenderCore] Cannot make Instance");

	{
		IRenderClass* vulkanObject = instanceInst.get();
		vulkanObject->create();
		vulkanClasses.emplace_back(vulkanObject);
	}

	// ===== 2. Surface (Instance + GLFWwindow 의존) =====
	surfaceInst = std::make_unique<Surface>(*instanceInst, window);

	if (!ENSURE(surfaceInst))
		throw std::runtime_error("[SeaRenderCore] Cannot make Surface");

	{
		IRenderClass* vulkanObject = surfaceInst.get();
		vulkanObject->create();
		vulkanClasses.emplace_back(vulkanObject);
	}

	// ===== 3. PhysicalDevice (Instance 의존) =====
	physicalDeviceInst = std::make_unique<PhysicalDevice>(*instanceInst);

	if (!ENSURE(physicalDeviceInst))
		throw std::runtime_error("[SeaRenderCore] Cannot make PhysicalDevice");

	{
		IRenderClass* vulkanObject = physicalDeviceInst.get();
		vulkanObject->create();
		vulkanClasses.emplace_back(vulkanObject);
	}

	// ===== 4. Device (PhysicalDevice 의존) =====
	deviceInst = std::make_unique<Device>(*physicalDeviceInst, *surfaceInst);

	if (!ENSURE(deviceInst))
		throw std::runtime_error("[SeaRenderCore] Cannot make Device");

	{
		IRenderClass* vulkanObject = deviceInst.get();
		vulkanObject->create();
		vulkanClasses.emplace_back(vulkanObject);
	}

	// ===== 5. SwapChain (Device + PhysicalDevice + Surface + GLFWwindow 의존) =====
	swapChainInst = std::make_unique<SwapChain>(*deviceInst, *physicalDeviceInst, *surfaceInst, window);

	if (!ENSURE(swapChainInst))
		throw std::runtime_error("[SeaRenderCore] Cannot make SwapChain");

	{
		IRenderClass* vulkanObject = swapChainInst.get();
		vulkanObject->create();
		vulkanClasses.emplace_back(vulkanObject);
	}

	// ===== 6. RenderPass (Device + SwapChain 의존) =====
	renderPassInst = std::make_unique<RenderPass>(*deviceInst, *swapChainInst);

	if (!ENSURE(renderPassInst))
		throw std::runtime_error("[SeaRenderCore] Cannot make RenderPass");

	{
		IRenderClass* vulkanObject = renderPassInst.get();
		vulkanObject->create();
		vulkanClasses.emplace_back(vulkanObject);
	}

	// ===== 7. DescriptorSetLayout (Device 의존) =====
	descriptorSetLayoutInst = std::make_unique<DescriptorSetLayout>(*deviceInst);

	if (!ENSURE(descriptorSetLayoutInst))
		throw std::runtime_error("[SeaRenderCore] Cannot make DescriptorSetLayout");

	{
		IRenderClass* vulkanObject = descriptorSetLayoutInst.get();
		vulkanObject->create();
		vulkanClasses.emplace_back(vulkanObject);
	}

	// ===== 8. PipelineLayout (Device + DescriptorSetLayout 의존) =====
	pipelineLayoutInst = std::make_unique<PipelineLayout>(*deviceInst, *descriptorSetLayoutInst);

	if (!ENSURE(pipelineLayoutInst))
		throw std::runtime_error("[SeaRenderCore] Cannot make PipelineLayout");

	{
		IRenderClass* vulkanObject = pipelineLayoutInst.get();
		vulkanObject->create();
		vulkanClasses.emplace_back(vulkanObject);
	}

	// ===== 9. GraphicsPipeline (Device + RenderPass + PipelineLayout + SwapChain 의존) =====
	graphicsPipelineInst = std::make_unique<GraphicsPipeline>(*deviceInst, *renderPassInst, *pipelineLayoutInst, *swapChainInst);

	if (!ENSURE(graphicsPipelineInst))
		throw std::runtime_error("[SeaRenderCore] Cannot make GraphicsPipeline");

	{
		IRenderClass* vulkanObject = graphicsPipelineInst.get();
		vulkanObject->create();
		vulkanClasses.emplace_back(vulkanObject);
	}

	// ===== 10. FrameBuffer (Device + SwapChain + RenderPass 의존) =====
	frameBufferInst = std::make_unique<FrameBuffer>(*deviceInst, *physicalDeviceInst, *swapChainInst, *renderPassInst);

	if (!ENSURE(frameBufferInst))
		throw std::runtime_error("[SeaRenderCore] Cannot make FrameBuffer");

	{
		IRenderClass* vulkanObject = frameBufferInst.get();
		vulkanObject->create();
		vulkanClasses.emplace_back(vulkanObject);
	}
}

void SeaRenderCore::update()
{
}

void SeaRenderCore::release() noexcept
{
	for (auto vulkanclassIt = vulkanClasses.rbegin(); vulkanclassIt != vulkanClasses.rend(); ++vulkanclassIt)
	{
		auto* vulkanclass = *vulkanclassIt;

		if (ENSURE(vulkanclass))
		{
			vulkanclass->destroy();
		}
	}
}
