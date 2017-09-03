#include "include\Application.hpp"

Application::Application(uint32_t width, uint32_t height, const std::string& title) :
	_width(width),
	_height(height),
	_title(title)
{

}

Application::~Application() {
}

void Application::errorCallback(int error, const char* message) {
	std::cerr << "error#: " << error << "; " << message << std::endl;
}

void Application::run() {
	initializeWindow();
	initializeVulkan();
	loop();
	cleanup();
}

void Application::initializeWindow() {
	glfwSetErrorCallback(errorCallback);

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, false);
	_window = glfwCreateWindow(_width, _height, _title.c_str(), nullptr, nullptr);
}

void Application::initializeVulkan() {
	createInstance();
}
void Application::createInstance() {
	VkApplicationInfo applicationInfo = {};
	applicationInfo.apiVersion = VK_API_VERSION_1_0;
	applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
	applicationInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
	applicationInfo.pApplicationName = _title.c_str();
	applicationInfo.pEngineName = "";
	applicationInfo.pNext = nullptr;
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

	uint32_t extensionsCount = 0;
	const char** extensions = glfwGetRequiredInstanceExtensions(&extensionsCount);
	VkInstanceCreateInfo instanceInfo = {};
	instanceInfo.enabledExtensionCount = extensionsCount;
	instanceInfo.enabledLayerCount = 0;
	instanceInfo.pApplicationInfo = &applicationInfo;
	instanceInfo.ppEnabledExtensionNames = extensions;
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

	VkResult result = vkCreateInstance(&instanceInfo, nullptr, &_vulkanInstance);
	if (result != VK_SUCCESS) {
		throw std::runtime_error(std::string("Couldn't create vulkanInstance: ") + std::to_string(result) + ";" + std::to_string(__LINE__));
	}


}

void Application::loop() {
	while (!glfwWindowShouldClose(_window)) {
		glfwPollEvents();
	}
}

void Application::cleanup() {
	vkDestroyInstance(_vulkanInstance, nullptr);
	glfwDestroyWindow(_window);
	glfwTerminate();
}