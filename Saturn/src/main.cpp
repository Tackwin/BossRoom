#include <iostream>
#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Managers/MemoryManager.hpp"
#include "Managers/AssetManager.hpp"

#include "System/Window.hpp"

#include "Graphics/Shader.hpp"
#include "Graphics/Transform.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Sprite.hpp"
#include "Graphics/VAO.hpp"

#include "Math/Matrix.hpp"


void play();

int main(int, char**) {
	MemoryManager::I().initialize_buffer(50'000'000);

	play();
	return 0;
}

void play() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	Window window(700u, 700u, "Saturn is up and running");

	glfwSwapInterval(0);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		LOG_PLACE;
		return;
	}

	AM::I().load_texture("wall", "res/images/wall.png");
	AM::I().load_shader("default", "res/shaders/shader");

	Sprite wall;
	wall.set_texture("wall");
	wall.set_shader("default");


	while (!window.should_close()) {
		static float a = 0.f;
		a += 0.00001f;
		glfwPollEvents();

		window.process_inputs();

		window.clear({ 0.1f, 0.15f, 0.2f, 1.f });

		wall.get_transform().set_rotation(a * pi);
		wall.render();

		window.swap_buffers();
	}

	glfwTerminate();
}