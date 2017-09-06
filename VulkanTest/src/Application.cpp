#include "../include/Application.hpp"

#include <algorithm>
#include <map>
#include <set>

#include "../include/ProxyFunctions.hpp"
#include "../include/Utils.hpp"

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
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createFrameBuffers();
	createCommandPool();
	createCommandBuffer();
	createSemaphores();
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
void Application::createRenderPass() {
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = _swapChainFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0u;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1u;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0u;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0u;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask =  VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
								VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	
	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = 1u;
	renderPassCreateInfo.pAttachments = &colorAttachment;
	renderPassCreateInfo.subpassCount = 1u;
	renderPassCreateInfo.pSubpasses = &subpass;
	renderPassCreateInfo.dependencyCount = 1u;
	renderPassCreateInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(_device, &renderPassCreateInfo, nullptr, &_renderPass) != VK_SUCCESS) {
		throw std::runtime_error("Couldn't create render pass :(");
	}
}
void Application::createGraphicsPipeline() {
	auto vertCode = Utils::readBytes("shaders/vert.spv");
	auto fragCode = Utils::readBytes("shaders/frag.spv");

	VkShaderModule vertShaderModule = createShaderModule(vertCode);
	VkShaderModule fragShaderModule = createShaderModule(fragCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
	vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputCreateInfo.vertexBindingDescriptionCount = 0u;
	vertexInputCreateInfo.pVertexBindingDescriptions = nullptr;
	vertexInputCreateInfo.vertexAttributeDescriptionCount = 0u;
	vertexInputCreateInfo.pVertexAttributeDescriptions = nullptr;

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {};
	inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.f;
	viewport.y = 0.f;
	viewport.width = (float)_swapChainExtent.width;
	viewport.height = (float)_swapChainExtent.height;
	viewport.maxDepth = 1.f;
	viewport.minDepth = 0.f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = _swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1u;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1u;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	if (vkCreatePipelineLayout(_device, &pipelineLayoutCreateInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("Couldn't create the pipeline layout :(");
	}

	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stageCount = 2u;
	pipelineCreateInfo.pStages = shaderStages;
	pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
	pipelineCreateInfo.pViewportState = &viewportState;
	pipelineCreateInfo.pRasterizationState = &rasterizer;
	pipelineCreateInfo.pMultisampleState = &multisampling;
	pipelineCreateInfo.pDepthStencilState = nullptr;
	pipelineCreateInfo.pColorBlendState = &colorBlending;
	pipelineCreateInfo.pDynamicState = nullptr;
	pipelineCreateInfo.layout = _pipelineLayout;
	pipelineCreateInfo.renderPass = _renderPass;
	pipelineCreateInfo.subpass = 0u;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineCreateInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1u, &pipelineCreateInfo, nullptr, &_graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("Couldn't create graphics pipeline :(");
	}

	vkDestroyShaderModule(_device, fragShaderModule, nullptr);
	vkDestroyShaderModule(_device, vertShaderModule, nullptr);
}
VkShaderModule Application::createShaderModule(const std::vector<char>& code) {
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("Couldn't create shader module! :(");
	}

	return shaderModule;
}
void Application::createFrameBuffers() {
	_swapChainFrameBuffers.resize(_swapChainImageViews.size());

	for (size_t i = 0u; i < _swapChainImageViews.size(); ++i) {
		VkImageView attachments[] = {
			_swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = _renderPass;
		framebufferCreateInfo.attachmentCount = 1u;
		framebufferCreateInfo.pAttachments = attachments;
		framebufferCreateInfo.width = _swapChainExtent.width;
		framebufferCreateInfo.height = _swapChainExtent.height;
		framebufferCreateInfo.layers = 1u;

		if (vkCreateFramebuffer(_device, &framebufferCreateInfo, nullptr, &_swapChainFrameBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("Couldn't create framebuffer :(");
		}
	}
}
void Application::createCommandPool() {
	QueueFamilyIndices indices = findQueueFamilies(_physicalDevice);

	VkCommandPoolCreateInfo poolCreateInfo = {};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolCreateInfo.queueFamilyIndex = indices.graphicsFamily;
	poolCreateInfo.flags = 0;

	if (vkCreateCommandPool(_device, &poolCreateInfo, nullptr, &_commandPool) != VK_SUCCESS) {
		throw std::runtime_error("Couldn't create command pool :(");
	}
}
void Application::createCommandBuffer() {
	_commandBuffers.resize(_swapChainFrameBuffers.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = _commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)_commandBuffers.size();

	if (vkAllocateCommandBuffers(_device, &allocInfo, _commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Couldn't create command buffers :(");
	}

	for (size_t i = 0u; i < _commandBuffers.size(); ++i) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(_commandBuffers[i], &beginInfo) != VK_SUCCESS) {
			std::cerr << "I dont know how you ended up here :/" << std::endl;
		}
		constexpr VkClearValue clearColor = { 0.f, 0.f, 0.f, 1.f };

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = _renderPass;
		renderPassInfo.framebuffer = _swapChainFrameBuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = _swapChainExtent;
		renderPassInfo.clearValueCount = 1u;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);
		vkCmdDraw(_commandBuffers[i], 3u, 1u, 0u, 0u);

		vkCmdEndRenderPass(_commandBuffers[i]);

		if (vkEndCommandBuffer(_commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("Couldn't record command buffer :(");
		}
	}
}
void Application::createSemaphores() {
	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	if (vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_renderFinishedSemaphore) != VK_SUCCESS) {
		throw std::runtime_error("Couldn't create semaphores :(");
	}
}


void Application::loop() {
	while (!glfwWindowShouldClose(_window)) {
		glfwPollEvents();
		render();
	}
	vkDeviceWaitIdle(_device);
}
void Application::render() {
	uint32_t imageIndex = 0u;
	vkAcquireNextImageKHR(_device, _swapChain, std::numeric_limits<uint32_t>::max(), _imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { _imageAvailableSemaphore };
	VkSemaphore signalSemaphore[] = { _renderFinishedSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1u;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1u;
	submitInfo.pCommandBuffers = &_commandBuffers[imageIndex];
	submitInfo.signalSemaphoreCount = 1u;
	submitInfo.pSignalSemaphores = signalSemaphore;

	if (vkQueueSubmit(_graphicQueue, 1u, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
		throw std::runtime_error("Couldn't csubmit draw command buffer :(");
	}

	VkSwapchainKHR swapChains[] = { _swapChain };
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1u;
	presentInfo.pWaitSemaphores = signalSemaphore;
	presentInfo.swapchainCount = 1u;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	if (vkQueuePresentKHR(_presentQueue, &presentInfo) != VK_SUCCESS) {
		throw std::runtime_error("Couldn't submit image to swap chain :(");
	}
	vkQueueWaitIdle(_presentQueue);
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
	vkDestroySemaphore(_device, _imageAvailableSemaphore, nullptr);
	vkDestroySemaphore(_device, _renderFinishedSemaphore, nullptr);
	vkDestroyCommandPool(_device, _commandPool, nullptr);
	for (size_t i = 0u; i < _swapChainFrameBuffers.size(); ++i) {
		vkDestroyFramebuffer(_device, _swapChainFrameBuffers[i], nullptr);
	}
	vkDestroyPipeline(_device, _graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
	vkDestroyRenderPass(_device, _renderPass, nullptr);
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
