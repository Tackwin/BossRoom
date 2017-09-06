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
	int32_t presentFamily = -1;

	bool isComplete() {
		return graphicsFamily >= 0 && presentFamily >= 0;
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
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
		void createSurface();
		void pickPhysicalDevice();
			uint32_t scorePhysicalDevice(const VkPhysicalDevice& device);
				bool checkDeviceExtensionsSupport(VkPhysicalDevice device);
			QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		void createLogicalDevice();
		void createSwapChain();
			SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
			VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
			VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& presentModes);
			VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		void createImageViews();
		void createGraphicsPipeline();
			VkShaderModule createShaderModule(const std::vector<char>& code);
		void createRenderPass();
		void createFrameBuffers();
		void createCommandPool();
		void createCommandBuffer();
		void createSemaphores();
	void loop();
		void render();
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
	VkQueue _presentQueue;

	VkSurfaceKHR _surface;

	VkSwapchainKHR _swapChain;
	std::vector<VkImage> _swapChainImages;
	VkFormat _swapChainFormat;
	VkExtent2D _swapChainExtent;

	VkRenderPass _renderPass;
	VkPipeline _graphicsPipeline;
	VkPipelineLayout _pipelineLayout;

	VkCommandPool _commandPool;
	std::vector<VkCommandBuffer> _commandBuffers;

	std::vector<VkImageView> _swapChainImageViews;
	std::vector<VkFramebuffer> _swapChainFrameBuffers;

	VkSemaphore _imageAvailableSemaphore;
	VkSemaphore _renderFinishedSemaphore;

	uint32_t _extensionsCount = 0u;
	std::vector<VkExtensionProperties> _availableExtensions;
	uint32_t _layerCount = 0u;
	std::vector<VkLayerProperties> _availableLayers;

	std::vector<const char*> _validationLayersRequired;
	std::vector<const char*> _deviceExtensionsRequired;
};

