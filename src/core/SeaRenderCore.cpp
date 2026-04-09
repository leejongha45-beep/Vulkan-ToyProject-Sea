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
	instanceInst = std::make_unique<Instance>();

	if (!ENSURE(instanceInst))
		throw std::runtime_error("Cannot make Instance");

	{
		IRenderClass* vulkanObject = instanceInst.get();
		vulkanObject->create();
		vulkanClasses.emplace_back(vulkanObject);
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
