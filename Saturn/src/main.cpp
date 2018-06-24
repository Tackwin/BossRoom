#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "System/Window.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/FrameBuffer.hpp"
#include "Graphics/Transform.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/VAO.hpp"

#include "Math/Matrix.hpp"

#include <thread>

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

	glfwSwapInterval(0);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		LOG_PLACE;
		return;
	}

	FrameBuffer frame({ 700u, 700u });
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Shader screen_shader;
	Shader shader;
	Texture texture;
	Transform transform;
	texture.set_parameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
	texture.set_parameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
	texture.set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	texture.set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	texture.load_file("res/images/wall.png");

	VAO vao = VAO::create_quad({ 1, 1 });

	screen_shader.load_vertex("res/shaders/passthrough.vertex.shader");
	screen_shader.load_fragment("res/shaders/fragment.shader");

	screen_shader.build_shaders();

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

		/*
		transform.set_origin({ cosf(a * pi) / 2.f, sinf(a * pi) / 2.f });
		transform.set_position({ cosf(a * pi) / 2.f, sinf(a * pi) / 2.f });
		transform.set_scale(Vector2f::createUnitVector(a * pi));
		transform.set_rotation(a * pi);*/
		
		transform.apply_to_shader(shader);

		frame.clear({ 0.1f, 0.15f, 0.2f, 1.f });
		frame.clear_buffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		frame.bind();
		shader.use();
		texture.bind();
		vao.bind();
		vao.render(6);


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		window.clear({ 0.1f, 0.15f, 0.2f, 1.f });
		window.set_viewport();

		transform.apply_to_shader(screen_shader);
		screen_shader.use();
		frame.render_texture();
		
		window.swap_buffers();

		std::this_thread::sleep_for(std::chrono::milliseconds(0));
	}

	glfwTerminate();
}