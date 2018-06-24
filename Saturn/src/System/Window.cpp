#include "System/Window.hpp"

#include <iostream>

#include <GLFW/glfw3.h>

Window::Window(u32 width, u32 height, std::string_view title) {
	_info.width = width;
	_info.height = height;
	_info.title = title;

	_window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
	if (!_window) {
		LOG_PLACE;
		glfwTerminate();
	}

	glfwMakeContextCurrent(_window);
	glViewport(0, 0, width, height);

	glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);
}

void Window::process_inputs() const {
	if (glfwGetKey(_window, GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(_window, true);
	}
}

void Window::clear(Vector4f color) const {
	glClearColor(COLOR_UNROLL(color));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool Window::should_close() const {
	return glfwWindowShouldClose(_window);
}
void Window::swap_buffers() const {
	glfwSwapBuffers(_window);
}

void Window::framebuffer_size_callback(
	GLFWwindow* , int width, int height
) {
	glViewport(0, 0, width, height);
}

void Window::set_viewport() const {
	glViewport(0, 0, _info.width, _info.height);
}

void Window::close() const {
	glfwSetWindowShouldClose(_window, true);
}
