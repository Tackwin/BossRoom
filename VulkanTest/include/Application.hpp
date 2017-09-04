#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#ifdef NDEBUG
	constexpr bool ENABLE_VALIDATION_LAYER = false;
#else
	constexpr bool ENABLE_VALIDATION_LAYER = true;
#endif

struct QueueFamilyIndices {
	int32_t graphicsFamily = -1;

	bool isComplete() {
		return graphicsFamily >= 0;
	}
};

class Application {
public:
	Application(uint32_t width, uint32_t height, const std::string& title);
	~Application();

	void run();

private:
	void initializeWindow();
	void initializeVulkan();
		void createInstance();
		void setupDebugCallback();
		void pullAvailableExtensions();
		void pullAvailableLayers();
		bool checkValidationLayerSupport();
		void pickPhysicalDevice();
			uint32_t scorePhysicalDevice(const VkPhysicalDevice& device);
			QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		void createLogicalDevice();
	void loop();
	void cleanup();

	std::vector<const char*> getRequiredExtensions();

	static void errorCallback(int, const char*);
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT, VkDebugReportObjectTypeEXT, uint64_t, size_t, int32_t, const char*, const char*, void*);

	GLFWwindow* _window;
	std::string _title;
	uint32_t _height;
	uint32_t _width;

	VkInstance _vulkanInstance;
	VkDebugReportCallbackEXT _debugCallback;

	uint32_t _physicalDeviceCount = 0u;
	std::vector<VkPhysicalDevice> _physicalDevices;
	VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;

	VkDevice _device;

	VkQueue _graphicQueue;

	uint32_t _extensionsCount = 0u;
	std::vector<VkExtensionProperties> _availableExtensions;
	uint32_t _layerCount = 0u;
	std::vector<VkLayerProperties> _availableLayers;

	std::vector<const char*> _validationLayers;
};

