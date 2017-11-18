#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "System/Window.hpp"
#include "Graphics/Shader.hpp"

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
		+0.5f, +0.5f, 0.0f,
		+0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f, +0.5f, 0.0f
	};

	u32 indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	u32 VAO = 0u;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	u32 EBO = 0u;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	u32 VBO = 0u;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	shader.load_vertex("res/shaders/vertex.shader");
	shader.load_fragment("res/shaders/fragment.shader");
	
	shader.build_shaders();

	while (!window.should_close()) {
		glfwPollEvents();

		window.process_inputs();

		window.clear();

		shader.use();

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		window.swap_buffers();
	}

	glfwTerminate();
	return 0;
}