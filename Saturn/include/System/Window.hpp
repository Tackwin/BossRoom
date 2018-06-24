#pragma once
#include <string_view>

#include "Concepts/Delete.hpp"

#include "Math/Vector.hpp"

#include "Common.hpp"

struct GLFWwindow;

struct WindowInfo {
	u32 width = 0u;
	u32 height = 0u;
	std::string title = "";
};

class Window : NoCopy, NoMove {
public:

	Window(u32 width, u32 height, std::string_view title);

	void set_viewport() const;

	void process_inputs() const;
	void clear(Vector4f color = {0.f, 0.f, 0.f, 0.f}) const;

	bool should_close() const;
	void swap_buffers() const;

	void close() const;

private:

	static void framebuffer_size_callback(
		GLFWwindow* window, int width, int height
	);

	GLFWwindow* _window;

	WindowInfo _info;
};
