#include "Vulkan_ToyProject_Sea.h"
#include "src/core/SeaEngine.h"

using namespace std;

int main()
{
	try
	{
		SeaEngine engine;
		engine.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
