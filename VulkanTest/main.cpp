#include "Application.hpp"

int main(int, char**) {

	Application app(900, 600, "Vulkan");

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::cin.get();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
