#include "include\Application.hpp"
#include "ProxyFunctions.hpp"
#include <algorithm>
#include <map>
#include <set>

Application::Application(uint32_t width, uint32_t height, const std::string& title) :
	_width(width),
	_height(height),
	_title(title),
	_validationLayersRequired({
		"VK_LAYER_LUNARG_standard_validation"
	}),
	_deviceExtensionsRequired({
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
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
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
}
void Application::createSurface() {
	if (glfwCreateWindowSurface(_vulkanInstance, _window, nullptr, &_surface) != VK_SUCCESS) {
		throw std::runtime_error("Couldn't create a surface :(");
	}
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
	instanceInfo.enabledLayerCount = ENABLE_VALIDATION_LAYER ? _validationLayersRequired.size() : 0u;
	instanceInfo.ppEnabledLayerNames = ENABLE_VALIDATION_LAYER ? _validationLayersRequired.data() : nullptr;
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
	_availableExtensions.resize(_extensionsCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &_extensionsCount, _availableExtensions.data());

	std::cout << "Available extensions" << std::endl;
	for (auto& a : _availableExtensions) {
		std::cout << "+\t" << a.extensionName << std::endl;
	}
	std::cout << std::endl;
}
void Application::pullAvailableLayers() {
	vkEnumerateInstanceLayerProperties(&_layerCount, nullptr);
	_availableLayers.resize(_layerCount);
	vkEnumerateInstanceLayerProperties(&_layerCount, _availableLayers.data());

	std::cout << "Available layers" << std::endl;
	for (auto& a : _availableLayers) {
		std::cout << "+\t" << a.layerName << std::endl;
	}
	std::cout << std::endl;
}
bool Application::checkValidationLayerSupport() {
	for (auto& v : _validationLayersRequired) {
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

	_physicalDevices.resize(_physicalDeviceCount);
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
	if (!checkDeviceExtensionsSupport(device))
		return score;
	if (const auto swapChainSupport = querySwapChainSupport(device); 
		swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty())
		return score;

	score += deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 10 : 0;
	score += deviceFeatures.geometryShader ? 1 : 0;
	return score;
}
bool Application::checkDeviceExtensionsSupport(VkPhysicalDevice device) {
	uint32_t extensionsCount = 0u;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionsCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(_deviceExtensionsRequired.begin(), _deviceExtensionsRequired.end());
	for (const auto& ext : availableExtensions) {
		requiredExtensions.erase(ext.extensionName);
	}
	return requiredExtensions.empty();
}
QueueFamilyIndices Application::findQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0u;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.data());

	int32_t i = 0;
	for (const auto& queueFamily : queueFamilyProperties) {
		VkBool32 present = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &present);
		
		if (queueFamily.queueCount > 0) {
			if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.graphicsFamily = i;

			if (present)
				indices.presentFamily = i;
		}
		if (indices.isComplete())
			break;
			
		i++;
	}
	return indices;
}
void Application::createLogicalDevice() {
	QueueFamilyIndices indice = findQueueFamilies(_physicalDevice);
	
	constexpr float priority = 1.f;
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<int32_t> uniqueQueueFamilies = { indice.graphicsFamily, indice.presentFamily };

	for (auto& queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueCount = 1u;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.pQueuePriorities = &priority;

		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures features = {};

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.enabledExtensionCount = (uint32_t)_deviceExtensionsRequired.size();
	deviceCreateInfo.ppEnabledExtensionNames = _deviceExtensionsRequired.data();
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
	deviceCreateInfo.pEnabledFeatures = &features;
	if constexpr(ENABLE_VALIDATION_LAYER) {
		deviceCreateInfo.enabledLayerCount = (uint32_t)_validationLayersRequired.size();
		deviceCreateInfo.ppEnabledLayerNames = _validationLayersRequired.data();
	} 
	else {
		deviceCreateInfo.enabledLayerCount = 0u;
	}

	if (vkCreateDevice(_physicalDevice, &deviceCreateInfo, nullptr, &_device) != VK_SUCCESS) {
		throw std::runtime_error("Couldn't create logical device :(");
	}

	vkGetDeviceQueue(_device, indice.graphicsFamily, 0, &_graphicQueue);
	vkGetDeviceQueue(_device, indice.presentFamily, 0, &_presentQueue);
}
void Application::createSwapChain() {
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(_physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = choosePresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1u;
	if (swapChainSupport.capabilities.maxImageCount != 0u && imageCount > swapChainSupport.capabilities.maxImageCount)
		imageCount = swapChainSupport.capabilities.maxImageCount;

	QueueFamilyIndices indice = findQueueFamilies(_physicalDevice);
	uint32_t queueFamilyIndicies[] = {
		(uint32_t)indice.graphicsFamily,
		(uint32_t)indice.presentFamily
	};
	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = _surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1u;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	if (indice.graphicsFamily != indice.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2u;
		createInfo.pQueueFamilyIndices = queueFamilyIndicies;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;
	
	if (vkCreateSwapchainKHR(_device, &createInfo, nullptr, &_swapChain) != VK_SUCCESS) {
		throw std::runtime_error("Couldn't create the swap chain :(");
	}

	vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, nullptr);
	_swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, _swapChainImages.data());

	_swapChainFormat = surfaceFormat.format;
	_swapChainExtent = extent;
}
SwapChainSupportDetails Application::querySwapChainSupport(VkPhysicalDevice device) {
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &details.capabilities);
	
	uint32_t formatCount = 0u;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);
	if (formatCount != 0u) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, details.formats.data());
	}

	uint32_t presentCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentCount, nullptr);
	if (presentCount != 0) {
		details.presentModes.resize(presentCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentCount, details.presentModes.data());
	}


	return details;
}
VkSurfaceFormatKHR Application::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	constexpr VkSurfaceFormatKHR BEST_FORMAT = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

	if (availableFormats.size() == 1 && availableFormats[0].colorSpace == VK_FORMAT_UNDEFINED) {
		return BEST_FORMAT;
	}
	for (const auto& format : availableFormats) {
		if (format.colorSpace == BEST_FORMAT.colorSpace &&
			format.format == BEST_FORMAT.format) {
			return BEST_FORMAT;
		}
	}
	return availableFormats[0];
}
VkPresentModeKHR Application::choosePresentMode(const std::vector<VkPresentModeKHR>& presentModes) {
	constexpr std::array<VkPresentModeKHR, 4> BEST_MODES = {
		VK_PRESENT_MODE_MAILBOX_KHR,
		VK_PRESENT_MODE_FIFO_KHR,
		VK_PRESENT_MODE_FIFO_RELAXED_KHR,
		VK_PRESENT_MODE_IMMEDIATE_KHR
	};
	
	for (const auto& mode : BEST_MODES) {
		for (const auto& presentMode : presentModes) {
			if (mode == presentMode)
				return presentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D Application::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	VkExtent2D extent = { _width, _height };
	extent.width = std::clamp(_width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
	extent.height = std::clamp(_height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
	return extent;
}
void Application::createImageViews() {
	_swapChainImageViews.resize(_swapChainImages.size());

	for (size_t i = 0u; i < _swapChainImages.size(); ++i) {
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = _swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = _swapChainFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0u;
		createInfo.subresourceRange.levelCount = 1u;
		createInfo.subresourceRange.baseArrayLayer = 0u;
		createInfo.subresourceRange.layerCount = 0u;

		if (vkCreateImageView(_device, &createInfo, nullptr, &_swapChainImageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("Couldn't create an image viex :(");
		}
	}
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
	for (auto& view : _swapChainImageViews)
		vkDestroyImageView(_device, view, nullptr);
	vkDestroySwapchainKHR(_device, _swapChain, nullptr);
	vkDestroySurfaceKHR(_vulkanInstance, _surface, nullptr);
	vkDestroyDevice(_device, nullptr);
	DestroyDebugReportCallbackEXT(_vulkanInstance, _debugCallback, nullptr);
	vkDestroyInstance(_vulkanInstance, nullptr);
	glfwDestroyWindow(_window);
	glfwTerminate();
}
