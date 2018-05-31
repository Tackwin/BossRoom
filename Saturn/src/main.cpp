#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Managers/MemoryManager.hpp"
#include "Managers/AssetManager.hpp"

#include "System/Window.hpp"

#include "Graphics/Shader.hpp"
#include "Graphics/Transform.hpp"
#include "Graphics/Texture.hpp"
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

	AM::I().add_texture("wall", "res/images/wall.png");
	auto& texture = AM::I().get_texture("wall");

	Shader shader;
	Transform transform;

	VAO vao;
	VAO::create_quad(vao, { 1, 1 });

	shader.load_vertex("res/shaders/vertex.shader");
	shader.load_fragment("res/shaders/fragment.shader");

	shader.build_shaders();

	transform.set_position({ 0, 0 });
	transform.set_rotation(0);
	transform.set_size({ 1, 1 });
	transform.set_scale({ 1, 1 });

	while (!window.should_close()) {
		static float a = 0.f;
		a += 0.00001f;
		glfwPollEvents();

		window.process_inputs();

		window.clear({ 0.1f, 0.15f, 0.2f, 1.f });

		transform.set_rotation(a * pi);

		transform.apply_to_shader(shader);

		texture.bind();
		vao.bind();
		vao.render(6);

		window.swap_buffers();
	}

	glfwTerminate();
}