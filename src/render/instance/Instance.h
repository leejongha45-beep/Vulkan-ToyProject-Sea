#pragma once

#include "../../../Vulkan_ToyProject_Sea.h"
#include "../IRenderClass.h"

namespace
{
#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif
} // namespace

class Instance final : public IRenderClass
{
public:
	Instance()			= default;
	virtual ~Instance() = default;

	Instance(const Instance&)			 = delete;
	Instance& operator=(const Instance&) = delete;

	Instance(Instance&&)			= delete;
	Instance& operator=(Instance&&) = delete;

public:
	const vk::raii::Instance* getInstance() const
	{
		return &instanceInst;
	}

private:
	virtual void create() override;
	virtual void destroy() override;

	vk::raii::Context contextInst;
	vk::raii::Instance instanceInst = nullptr;

private:
	std::vector<const char*> getRequiredExtensions() const;
};