#include "SeaEngine.h"
#include "SeaRenderCore.h"

SeaEngine::SeaEngine()
{
}

SeaEngine::~SeaEngine()
{
	release();
}

void SeaEngine::run()
{
	initWindow();

	initialize();

	update();
}

void SeaEngine::initWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(windowWidth, windowHeight, "Sea Renderer", nullptr, nullptr);
	assert(window);
}

void SeaEngine::initialize()
{
	if (!renderCoreInst)
	{
		renderCoreInst = std::make_unique<SeaRenderCore>();
		if (ENSURE(renderCoreInst))
		{
			renderCoreInst->initialize();
		}
	}
}

void SeaEngine::update()
{
	double previousTime				 = glfwGetTime();
	constexpr double targetFrameTime = 1.0 / 60.0;

	while (!glfwWindowShouldClose(window))
	{
		double currentTime = glfwGetTime();
		double elapsedTime = currentTime - previousTime;

		if (elapsedTime < targetFrameTime)
			continue;

		float deltaSeconds = static_cast<float>(elapsedTime);
		previousTime	   = currentTime;

		glfwPollEvents();
	}
}

void SeaEngine::release()
{
	if (window)
	{
		glfwDestroyWindow(window);
	}
	glfwTerminate();
}
