#include "include\Application.hpp"
#include "ProxyFunctions.hpp"
#include <algorithm>
#include <map>

Application::Application(uint32_t width, uint32_t height, const std::string& title) :
	_width(width),
	_height(height),
	_title(title),
	_validationLayers({
		"VK_LAYER_LUNARG_standard_validation"
	})
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
	setupDebugCallback();
	pickPhysicalDevice();
	createLogicalDevice();
}
void Application::createInstance() {
	pullAvailableExtensions();
	pullAvailableLayers();
	if (ENABLE_VALIDATION_LAYER && !checkValidationLayerSupport()) {
		throw std::runtime_error("Validation layers requested but not present");
	}

	VkApplicationInfo applicationInfo = {};
	applicationInfo.apiVersion = VK_API_VERSION_1_0;
	applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
	applicationInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
	applicationInfo.pApplicationName = _title.c_str();
	applicationInfo.pEngineName = "";
	applicationInfo.pNext = nullptr;
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

	auto extensions = getRequiredExtensions();
	VkInstanceCreateInfo instanceInfo = {};
	instanceInfo.enabledExtensionCount = extensions.size();
	instanceInfo.enabledLayerCount = ENABLE_VALIDATION_LAYER ? _validationLayers.size() : 0u;
	instanceInfo.ppEnabledLayerNames = ENABLE_VALIDATION_LAYER ? _validationLayers.data() : nullptr;
	instanceInfo.pApplicationInfo = &applicationInfo;
	instanceInfo.ppEnabledExtensionNames = extensions.data();
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

	VkResult result = vkCreateInstance(&instanceInfo, nullptr, &_vulkanInstance);
	if (result != VK_SUCCESS) {
		throw std::runtime_error(std::string("Couldn't create vulkanInstance :( : ") + std::to_string(result));
	}
}
void Application::setupDebugCallback() {
	if constexpr(!ENABLE_VALIDATION_LAYER) return;

	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = debugCallback;

	if (CreateDebugReportCallbackEXT(_vulkanInstance, &createInfo, nullptr, &_debugCallback) != VK_SUCCESS) {
		throw std::runtime_error("Couldn't setup the callback :(");
	}
}
void Application::pullAvailableExtensions() {
	vkEnumerateInstanceExtensionProperties(nullptr, &_extensionsCount, nullptr);
	_availableExtensions = std::vector<VkExtensionProperties>(_extensionsCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &_extensionsCount, _availableExtensions.data());

	std::cout << "Available extensions" << std::endl;
	for (auto& a : _availableExtensions) {
		std::cout << "+\t" << a.extensionName << std::endl;
	}
	std::cout << std::endl;
}
void Application::pullAvailableLayers() {
	vkEnumerateInstanceLayerProperties(&_layerCount, nullptr);
	_availableLayers = std::vector<VkLayerProperties>(_layerCount);
	vkEnumerateInstanceLayerProperties(&_layerCount, _availableLayers.data());

	std::cout << "Available layers" << std::endl;
	for (auto& a : _availableLayers) {
		std::cout << "+\t" << a.layerName << std::endl;
	}
	std::cout << std::endl;
}
bool Application::checkValidationLayerSupport() {
	for (auto& v : _validationLayers) {
		bool flag = false;

		for (auto& a : _availableLayers) {
			if (std::strcmp(v, a.layerName) == 0) {
				flag = true;
				break;
			}
		}

		if (!flag)
			return false;
	}
	return true;
}
void Application::pickPhysicalDevice() {
	vkEnumeratePhysicalDevices(_vulkanInstance, &_physicalDeviceCount, nullptr);
	if (_physicalDeviceCount == 0)
		throw std::runtime_error("Couldn't find a GPU supporting vulkan :(");

	_physicalDevices = std::vector<VkPhysicalDevice>(_physicalDeviceCount);
	vkEnumeratePhysicalDevices(_vulkanInstance, &_physicalDeviceCount, _physicalDevices.data());

	std::map<uint32_t, const VkPhysicalDevice*> devices;
	for (const auto& device : _physicalDevices) {
		const auto& score = scorePhysicalDevice(device);
		if (score == 0u)
			continue;

		devices[score] = &device;
	}
	_physicalDevice = *(devices.begin()->second);
	if (_physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("Couldn't find a suitable GPU :(");
	}
}
uint32_t Application::scorePhysicalDevice(const VkPhysicalDevice& device) {
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	uint32_t score = 0u;
	if (!findQueueFamilies(device).isComplete())
		return score;

	score += deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 10 : 0;
	score += deviceFeatures.geometryShader ? 1 : 0;
	return score;
}
QueueFamilyIndices Application::findQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0u;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.data());

	int32_t i = 0;
	for (const auto& queueFamily : queueFamilyProperties) {
		if (queueFamily.queueCount > 0 && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
			indices.graphicsFamily = i;
			break;
		}
		
		i++;
	}
	return indices;
}

void Application::createLogicalDevice() {
	QueueFamilyIndices indice = findQueueFamilies(_physicalDevice);
	
	constexpr float priority = 1.f;
	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueCount = 1u;
	queueCreateInfo.queueFamilyIndex = indice.graphicsFamily;
	queueCreateInfo.pQueuePriorities = &priority;

	VkPhysicalDeviceFeatures features = {};

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	deviceCreateInfo.queueCreateInfoCount = 1u;
	deviceCreateInfo.pEnabledFeatures = &features;
	deviceCreateInfo.enabledExtensionCount = 0;
	if constexpr(ENABLE_VALIDATION_LAYER) {
		deviceCreateInfo.enabledLayerCount = (uint32_t)_validationLayers.size();
		deviceCreateInfo.ppEnabledLayerNames = _validationLayers.data();
	} 
	else {
		deviceCreateInfo.enabledLayerCount = 0u;
	}

	if (vkCreateDevice(_physicalDevice, &deviceCreateInfo, nullptr, &_device) != VK_SUCCESS) {
		throw std::runtime_error("Couldn't create logical device :(");
	}

	vkGetDeviceQueue(_device, indice.graphicsFamily, 0, &_graphicQueue);
}


void Application::loop() {
	while (!glfwWindowShouldClose(_window)) {
		glfwPollEvents();
	}
}

std::vector<const char*> Application::getRequiredExtensions() {
	std::vector<const char*> result;

	uint32_t glfwExtensionsCount = 0u;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
	for (uint32_t i = 0u; i < glfwExtensionsCount; ++i) {
		result.push_back(glfwExtensions[i]);
	}
	if constexpr (ENABLE_VALIDATION_LAYER) {
		result.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	return result;
}

VKAPI_ATTR VkBool32 VKAPI_CALL Application::debugCallback(
	VkDebugReportFlagsEXT, VkDebugReportObjectTypeEXT, uint64_t, size_t, int32_t, const char*, const char* msg, void*
) 
{
	std::cerr << "Validation layer: " << msg << std::endl;

	return VK_FALSE;
}

void Application::cleanup() {
	vkDestroyDevice(_device, nullptr);
	DestroyDebugReportCallbackEXT(_vulkanInstance, _debugCallback, nullptr);
	vkDestroyInstance(_vulkanInstance, nullptr);
	glfwDestroyWindow(_window);
	glfwTerminate();
}
