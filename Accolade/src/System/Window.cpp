#include "System/Window.hpp"

#include <functional>

Window::Window() :
	Window(900u, 600u, "") {
	
}

Window::Window(u32 width, u32 height, const std::string& title) :
	_height(height),
	_width(width),
	_title(title)
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

	if (!_window) {
		printf("Couldn't create window\n");
		assert(true);
	}

	glfwSetWindowUserPointer(_window, (void*)this);
	glfwMakeContextCurrent(_window);
	glfwSetErrorCallback(Window::error_callback);
	glfwSetKeyCallback(_window, Window::key_callback);
}

Window::~Window() {
	glfwDestroyWindow(_window);
}

bool Window::isOpen() const {
	return !glfwWindowShouldClose(_window);
}

void Window::pushEvent(const Event& event) {
	_events.push(event);
}

const Window::Event& Window::pullEvent() {
	if (_events.empty()) {
		return { Event::Type::NONE };
	}
	auto e = _events.top();
	_events.pop();
	return e;
}

void Window::error_callback(int error, const char* msg) {
	printf("Error: %d \n\t %s", error, msg);
}
void Window::key_callback(
	GLFWwindow* GLFWwindow, int key, int scancode, int action, int mods
) {
	Window* window = (Window*)glfwGetWindowUserPointer(GLFWwindow);

	Event e;
	switch (action) {
	case GLFW_PRESS:
		e.type = Event::Type::KEY_PRESSED;
		break;
	case GLFW_REPEAT:
		e.type = Event::Type::KEY_REPEAT;
		break;
	case GLFW_RELEASE:
		e.type = Event::Type::KEY_RELEASED;
		break;
	default:
		e.type = Event::Type::NONE;
	}
	e.id = key;
	e.mods = mods;

	window->pushEvent(e);
}
