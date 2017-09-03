#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <exception>
#include <iostream>
#include <string>

class Application {
public:
	Application(uint32_t width, uint32_t height, const std::string& title);
	~Application();

	void run();

private:
	void initializeWindow();
	void initializeVulkan();
		void createInstance();
	void loop();
	void cleanup();


	static void errorCallback(int, const char*);

	GLFWwindow* _window;
	std::string _title;
	uint32_t _height;
	uint32_t _width;

	VkInstance _vulkanInstance;
};

