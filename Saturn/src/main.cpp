#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "System/Window.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VAO.hpp"

int main(int, char**) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	Window window(800u, 600u, "Saturn is up and running");

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		LOG_PLACE;
		return -1;
	}
	
	Shader shader;

	float vertices[] = {
		+0.5f, +0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		+0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f,
		-0.5f, +0.5f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	u32 indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	VAO v;
	v.set_element_data(indices, 6u);
	v.set_vertex_data(vertices, 24u);
	v.set_vertex_attrib(0u, 3u, false, 6u, 0u);
	v.set_vertex_attrib(1u, 3u, false, 6u, 3u);
	v.enable_vertex_attrib(0u);
	v.enable_vertex_attrib(1u);
	
	shader.load_vertex("res/shaders/vertex.shader");
	shader.load_fragment("res/shaders/fragment.shader");
	
	shader.build_shaders();

	while (!window.should_close()) {
		glfwPollEvents();

		window.process_inputs();

		window.clear({0.1f, 0.15f, 0.2f, 1.f});

		shader.use();

		v.bind();
		v.render(6);

		window.swap_buffers();
	}

	glfwTerminate();
	return 0;
}