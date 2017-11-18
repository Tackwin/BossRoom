#include <iostream>
#include <sstream>
#include <fstream>

#include "Graphics/Shader.hpp"

#include <glad/glad.h>

Shader::Shader() {
	_shaderInfo.vertexId = glCreateShader(GL_VERTEX_SHADER);
	_shaderInfo.fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
}

Shader::~Shader() {
	glDeleteShader(_shaderInfo.vertexId);
	glDeleteShader(_shaderInfo.fragmentId);
	glDeleteProgram(_shaderInfo.programId);
}

void Shader::load_vertex(const std::string& path) {
	std::ifstream file(path);

	_ASSERT_EXPR(file.is_open(), ("Can't find shader: " + path).c_str());

	file.seekg(0, file.end);
	size_t size = (size_t)file.tellg();
	file.seekg(0, file.beg);

	_shaderInfo.vertexSource.resize(size);
	file.read(_shaderInfo.vertexSource.data(), size);
}
void Shader::load_fragment(const std::string& path) {
	std::ifstream file(path);

	_ASSERT_EXPR(file.is_open(), ("Can't find shader: " + path).c_str());

	file.seekg(0, file.end);
	size_t size = (size_t)file.tellg();
	file.seekg(0, file.beg);

	_shaderInfo.fragmentSource.resize(size);
	file.read(_shaderInfo.fragmentSource.data(), size);
}

void Shader::build_shaders() {
	auto vertexSource = _shaderInfo.vertexSource.data();
	auto fragmentSource = _shaderInfo.fragmentSource.data();

	glShaderSource(_shaderInfo.vertexId, 1, &vertexSource, nullptr);
	glCompileShader(_shaderInfo.vertexId);
	
	check_shader_error(_shaderInfo.vertexId);

	glShaderSource(_shaderInfo.fragmentId, 1, &fragmentSource, nullptr);
	glCompileShader(_shaderInfo.fragmentId);

	check_shader_error(_shaderInfo.fragmentId);

	_shaderInfo.programId = glCreateProgram();
	glAttachShader(_shaderInfo.programId, _shaderInfo.vertexId);
	glAttachShader(_shaderInfo.programId, _shaderInfo.fragmentId);
	glLinkProgram(_shaderInfo.programId);

	glDeleteShader(_shaderInfo.vertexId);
	glDeleteShader(_shaderInfo.fragmentId);
}

void Shader::use() const {
	glUseProgram(_shaderInfo.programId);
}

void Shader::check_shader_error(u32 shader) {
	i32 success = 0;
	std::string log(512, 0);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, nullptr, log.data());
		std::cout << "Error compilation shader" << std::endl << log << std::endl;
	}
}

void Shader::check_program_error(u32 program) {
	i32 success = 0;
	std::string log(512, 0);
	glGetProgramiv(program, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, nullptr, log.data());
		std::cout << "Error compilation shader" << std::endl << log << std::endl;
	}
}
