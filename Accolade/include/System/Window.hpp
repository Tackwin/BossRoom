#pragma once
#include <stack>
#include <GLFW/glfw3.h>

#include "Math/Vector.hpp"

#include "Common.hpp"


class Window {
public:

	struct Event {
		enum class Type : u08 {
			NONE,
			KEY_PRESSED,
			KEY_REPEAT,
			KEY_RELEASED,
			MOUSE_PRESSED,
			MOUSE_REPEAT,
			MOUSE_RELEASED,
			MAX
		};

		Type type;
		i32 id;
		i32 mods;
		Vector2f screen_pos;

		operator bool() {
			return type == Type::NONE;
		}
	};

	Window();
	Window(u32 width, u32 height, const std::string& title);
	~Window();

	bool isOpen() const;
	Event pullEvent();

	static void error_callback(int error, const char* msg);
	static void key_callback(
		GLFWwindow* GLFWwindow, int key, int, int action, int mods
	);

private:
	void pushEvent(const Event& event);

	u32 _height = 0u;
	u32 _width = 0u;
	std::string _title = "";

	GLFWwindow* _window = nullptr;

	std::stack<Event> _events;
};