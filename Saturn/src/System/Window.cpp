#include "System/Window.hpp"

#include <iostream>

Window::Window(u32 width, u32 height, std::string_view title) {
	_windowInfo.width = width;
	_windowInfo.height = height;
	_windowInfo.title = title;

	_window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
	if (!_window) {
		LOG_PLACE;
		glfwTerminate();
	}

	glfwMakeContextCurrent(_window);
	glViewport(0, 0, width, height);

	glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback);
}

void Window::process_inputs() {
	if (glfwGetKey(_window, GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(_window, true);
	}
}

void Window::clear(Vector4f color) {

	glClearColor(COLOR_UNROLL(color));
	glClear(GL_COLOR_BUFFER_BIT);
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
