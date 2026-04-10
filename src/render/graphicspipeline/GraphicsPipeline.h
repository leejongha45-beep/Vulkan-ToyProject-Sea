#pragma once

#include "../../../Vulkan_ToyProject_Sea.h"
#include "../IRenderClass.h"

class GraphicsPipeline final : public IRenderClass
{
public:
	explicit GraphicsPipeline(
		const class Device&         deviceRef,
		const class RenderPass&     renderPassRef,
		const class PipelineLayout& pipelineLayoutRef,
		const class SwapChain&      swapChainRef);
	virtual ~GraphicsPipeline() = default;

	GraphicsPipeline()                                   = delete;
	GraphicsPipeline(const GraphicsPipeline&)             = delete;
	GraphicsPipeline& operator=(const GraphicsPipeline&)  = delete;
	GraphicsPipeline(GraphicsPipeline&&)                  = delete;
	GraphicsPipeline& operator=(GraphicsPipeline&&)       = delete;

public:
	const vk::raii::Pipeline* getPipeline() const
	{
		return &graphicsPipelineInst;
	}

private:
	virtual void create() override;
	virtual void destroy() override;

	vk::raii::ShaderModule createShaderModule(const std::vector<char>& code) const;
	static std::vector<char> readShaderFile(const std::string& filePath);

	const class Device&         deviceRef;
	const class RenderPass&     renderPassRef;
	const class PipelineLayout& pipelineLayoutRef;
	const class SwapChain&      swapChainRef;

	vk::raii::Pipeline graphicsPipelineInst = nullptr;
};
