#pragma once
#include <string_view>

#include <GLFW/glfw3.h>

#include "Math/Vector.hpp"

#include "Common.hpp"

struct WindowInfo {
	u32 width = 0u;
	u32 height = 0u;
	std::string title = "";
};

class Window {
public:

	Window(u32 width, u32 height, std::string_view title);

	void process_inputs();
	void clear(Vector4f color = {0.f, 0.f, 0.f, 0.f});

	bool should_close() const;
	void swap_buffers() const;

private:

	static void framebuffer_size_callback(
		GLFWwindow* window, int width, int height
	);

	GLFWwindow* _window;

	WindowInfo _windowInfo;
};
