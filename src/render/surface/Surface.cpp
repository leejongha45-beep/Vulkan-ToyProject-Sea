#include "Surface.h"
#include "../instance/Instance.h"

Surface::Surface(const Instance& instance, GLFWwindow* window) : instanceRef(instance), windowRef(window)
{
}

void Surface::create()
{
	if (!ENSURE(instanceRef.getInstance() != nullptr))
		throw std::runtime_error("[Surface] Instance is not valid");

	if (!ENSURE(windowRef != nullptr))
		throw std::runtime_error("[Surface] GLFWwindow is not valid");

	VkSurfaceKHR rawSurface = VK_NULL_HANDLE;
	VkResult result =
		glfwCreateWindowSurface(static_cast<VkInstance>(**instanceRef.getInstance()), windowRef, nullptr, &rawSurface);

	if (!ENSURE(result == VK_SUCCESS))
		throw std::runtime_error("[Surface] Failed to create window surface");

	surfaceInst = vk::raii::SurfaceKHR(*instanceRef.getInstance(), rawSurface);
}

void Surface::destroy()
{
	if (*surfaceInst != VK_NULL_HANDLE)
		surfaceInst.clear();
}
