#pragma once

#include "../../../Vulkan_ToyProject_Sea.h"
#include "../IRenderClass.h"

class Surface final : public IRenderClass
{
public:
	explicit Surface(const class Instance& instanceRef, GLFWwindow* windowRef);
	virtual ~Surface() = default;

	Surface()						   = delete;
	Surface(const Surface&)			   = delete;
	Surface& operator=(const Surface&) = delete;
	Surface(Surface&&)				   = delete;
	Surface& operator=(Surface&&)	   = delete;

public:
	const vk::raii::SurfaceKHR* getSurface() const
	{
		return &surfaceInst;
	}

private:
	virtual void create() override;
	virtual void destroy() override;

	const class Instance& instanceRef;
	GLFWwindow* windowRef = nullptr;

	vk::raii::SurfaceKHR surfaceInst = nullptr;
};
