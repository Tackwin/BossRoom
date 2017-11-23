#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "System/Window.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/VAO.hpp"

#include "Math/Matrix.hpp"

void play();

int main(int, char**) {
	play();
	return 0;
}

void play() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	Window window(700u, 700u, "Saturn is up and running");

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		LOG_PLACE;
		return;
	}

	Shader shader;
	Texture texture;
	texture.set_parameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
	texture.set_parameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
	texture.set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	texture.set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	texture.load_file("res/images/wall.png");

	float vertices[] = {
		+0.5f, +0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		+0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, +0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f
	};

	u32 indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	VAO vao;
	vao.set_element_data(indices, 6u);
	vao.set_vertex_data(vertices, 32u);
	vao.set_vertex_attrib(0u, 3u, false, 8u, 0u);
	vao.set_vertex_attrib(1u, 3u, false, 8u, 3u);
	vao.set_vertex_attrib(2u, 2u, false, 8u, 6u);
	vao.enable_vertex_attrib(0u);
	vao.enable_vertex_attrib(1u);
	vao.enable_vertex_attrib(2u);

	shader.load_vertex("res/shaders/vertex.shader");
	shader.load_fragment("res/shaders/fragment.shader");

	shader.build_shaders();

	while (!window.should_close()) {
		static float a = 0.f;
		a += 0.003f;
		glfwPollEvents();

		window.process_inputs();

		window.clear({ 0.1f, 0.15f, 0.2f, 1.f });

		shader.use();
		shader.set_uni_mat4f(
			"t",
			Mat4f::translation({cosf(pi * a) / 2.f, sinf(pi * a) / 2.f, 0}) *
			Mat4f::rotation({ 0, 0, 1 }, pi * a * 3)
		);


		texture.bind();
		vao.bind();
		vao.render(6);

		window.swap_buffers();
	}

	glfwTerminate();
}