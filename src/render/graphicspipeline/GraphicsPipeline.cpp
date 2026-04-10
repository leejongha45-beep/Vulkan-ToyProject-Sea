#include "GraphicsPipeline.h"
#include "../device/Device.h"
#include "../renderpass/RenderPass.h"
#include "../pipelinelayout/PipelineLayout.h"
#include "../swapchain/SwapChain.h"

#include <fstream>

GraphicsPipeline::GraphicsPipeline(
	const Device&         device,
	const RenderPass&     renderPass,
	const PipelineLayout& pipelineLayout,
	const SwapChain&      swapChain)
	: deviceRef(device)
	, renderPassRef(renderPass)
	, pipelineLayoutRef(pipelineLayout)
	, swapChainRef(swapChain)
{
}

void GraphicsPipeline::create()
{
	if (!ENSURE(deviceRef.getDevice() != nullptr))
		throw std::runtime_error("[GraphicsPipeline] Device is not valid");

	const auto& device         = *deviceRef.getDevice();
	const auto& renderPass     = *renderPassRef.getRenderPass();
	const auto& pipelineLayout = *pipelineLayoutRef.getPipelineLayout();

	// 셰이더 로딩
	auto vertShaderCode = readShaderFile(std::string(SHADER_DIR) + "vert.spv");
	auto fragShaderCode = readShaderFile(std::string(SHADER_DIR) + "frag.spv");

	auto vertShaderModule = createShaderModule(vertShaderCode);
	auto fragShaderModule = createShaderModule(fragShaderCode);

	// 셰이더 스테이지
	std::array shaderStages = {
		vk::PipelineShaderStageCreateInfo{
			.stage  = vk::ShaderStageFlagBits::eVertex,
			.module = *vertShaderModule,
			.pName  = "main",
		},
		vk::PipelineShaderStageCreateInfo{
			.stage  = vk::ShaderStageFlagBits::eFragment,
			.module = *fragShaderModule,
			.pName  = "main",
		},
	};

	// Vertex input (TODO: 버텍스 바인딩/어트리뷰트 설정)
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
		.vertexBindingDescriptionCount   = 0,
		.vertexAttributeDescriptionCount = 0,
	};

	// Input assembly
	vk::PipelineInputAssemblyStateCreateInfo inputAssembly{
		.topology               = vk::PrimitiveTopology::eTriangleList,
		.primitiveRestartEnable = vk::False,
	};

	// Viewport & Scissor (동적 상태로 설정)
	vk::PipelineViewportStateCreateInfo viewportState{
		.viewportCount = 1,
		.scissorCount  = 1,
	};

	// Rasterizer
	vk::PipelineRasterizationStateCreateInfo rasterizer{
		.depthClampEnable        = vk::False,
		.rasterizerDiscardEnable = vk::False,
		.polygonMode             = vk::PolygonMode::eFill,
		.cullMode                = vk::CullModeFlagBits::eBack,
		.frontFace               = vk::FrontFace::eCounterClockwise,
		.depthBiasEnable         = vk::False,
		.lineWidth               = 1.0f,
	};

	// Multisampling
	vk::PipelineMultisampleStateCreateInfo multisampling{
		.rasterizationSamples = vk::SampleCountFlagBits::e1,
		.sampleShadingEnable  = vk::False,
	};

	// Depth stencil
	vk::PipelineDepthStencilStateCreateInfo depthStencil{
		.depthTestEnable       = vk::True,
		.depthWriteEnable      = vk::True,
		.depthCompareOp        = vk::CompareOp::eLess,
		.depthBoundsTestEnable = vk::False,
		.stencilTestEnable     = vk::False,
	};

	// Color blending
	vk::PipelineColorBlendAttachmentState colorBlendAttachment{
		.blendEnable    = vk::False,
		.colorWriteMask = vk::ColorComponentFlagBits::eR
						| vk::ColorComponentFlagBits::eG
						| vk::ColorComponentFlagBits::eB
						| vk::ColorComponentFlagBits::eA,
	};

	vk::PipelineColorBlendStateCreateInfo colorBlending{
		.logicOpEnable   = vk::False,
		.attachmentCount = 1,
		.pAttachments    = &colorBlendAttachment,
	};

	// Dynamic state (뷰포트/시저 동적 변경 가능)
	constexpr std::array dynamicStates = {
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor,
	};

	vk::PipelineDynamicStateCreateInfo dynamicState{
		.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
		.pDynamicStates    = dynamicStates.data(),
	};

	// 파이프라인 생성
	vk::GraphicsPipelineCreateInfo pipelineInfo{
		.stageCount          = static_cast<uint32_t>(shaderStages.size()),
		.pStages             = shaderStages.data(),
		.pVertexInputState   = &vertexInputInfo,
		.pInputAssemblyState = &inputAssembly,
		.pViewportState      = &viewportState,
		.pRasterizationState = &rasterizer,
		.pMultisampleState   = &multisampling,
		.pDepthStencilState  = &depthStencil,
		.pColorBlendState    = &colorBlending,
		.pDynamicState       = &dynamicState,
		.layout              = *pipelineLayout,
		.renderPass          = *renderPass,
		.subpass             = 0,
	};

	graphicsPipelineInst = vk::raii::Pipeline(device, nullptr, pipelineInfo);

	std::cerr << "[GraphicsPipeline] Created" << std::endl;
}

void GraphicsPipeline::destroy()
{
	if (*graphicsPipelineInst != VK_NULL_HANDLE)
		graphicsPipelineInst.clear();
}

vk::raii::ShaderModule GraphicsPipeline::createShaderModule(const std::vector<char>& code) const
{
	const auto& device = *deviceRef.getDevice();

	vk::ShaderModuleCreateInfo createInfo{
		.codeSize = code.size(),
		.pCode    = reinterpret_cast<const uint32_t*>(code.data()),
	};

	return vk::raii::ShaderModule(device, createInfo);
}

std::vector<char> GraphicsPipeline::readShaderFile(const std::string& filePath)
{
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("[GraphicsPipeline] Cannot open shader file: " + filePath);
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), static_cast<std::streamsize>(fileSize));

	return buffer;
}
