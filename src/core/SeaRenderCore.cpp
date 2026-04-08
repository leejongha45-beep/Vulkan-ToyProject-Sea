#include "SeaRenderCore.h"
#include "../render/IRenderClass.h"
#include "../render/instance/Instance.h"

SeaRenderCore::SeaRenderCore()
{
}

SeaRenderCore::~SeaRenderCore()
{
	release();
}

void SeaRenderCore::initialize()
{
	if (!instanceInst)
	{
		instanceInst = std::make_unique<Instance>();
		if (ENSURE(instanceInst))
		{
			vulkanClasses.emplace_back(instanceInst.get());
		}
	}

	for (auto* vulkanclass : vulkanClasses)
	{
		if (!ENSURE(vulkanclass))
			return;

		vulkanclass->create();
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
