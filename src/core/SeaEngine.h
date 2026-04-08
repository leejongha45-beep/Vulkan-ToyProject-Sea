#pragma once

#include "../../Vulkan_ToyProject_Sea.h"

class SeaEngine
{
public:
	SeaEngine();
	~SeaEngine();

	SeaEngine(const SeaEngine&)			   = delete;
	SeaEngine& operator=(const SeaEngine&) = delete;

	SeaEngine(SeaEngine&&)			  = delete;
	SeaEngine& operator=(SeaEngine&&) = delete;

public:
	void run();

private:
	void initWindow();
	void initialize();
	void update();
	void release();

private:
	GLFWwindow* window	  = nullptr;
	uint32_t windowWidth  = 1280;
	uint32_t windowHeight = 720;

	std::unique_ptr<class SeaRenderCore> renderCoreInst = nullptr;
};