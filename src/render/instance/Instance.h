#pragma once

#include "../../../Vulkan_ToyProject_Sea.h"
#include "../IRenderClass.h"

class Instance final : public IRenderClass
{
public:
	Instance()			= default;
	virtual ~Instance() = default;

	Instance(const Instance&)			 = delete;
	Instance& operator=(const Instance&) = delete;

	Instance(Instance&&)			= delete;
	Instance& operator=(Instance&&) = delete;

private:
	virtual void create() override;
	virtual void destroy() override;

	vk::raii::Context context;
	vk::raii::Instance instance;
};