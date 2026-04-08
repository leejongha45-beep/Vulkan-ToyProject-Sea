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
	void initialize();
	void update();

private:
	void release() noexcept;

private:
	std::unique_ptr<class Instance> instanceInst;

	std::vector<class IRenderClass*> vulkanClasses;
};