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

bool Shader::load_vertex(const std::string& path) {
	std::ifstream file(path);

	_ASSERT_EXPR(file.is_open(), ("Can't find shader: " + path).c_str());
	if (!file.is_open()) return false;

	file.seekg(0, file.end);
	size_t size = (size_t)file.tellg();
	file.seekg(0, file.beg);

	_shaderInfo.vertexSource.resize(size);
	file.read(_shaderInfo.vertexSource.data(), size);

	return true;
}
bool Shader::load_fragment(const std::string& path) {
	std::ifstream file(path);

	_ASSERT_EXPR(file.is_open(), ("Can't find shader: " + path).c_str());
	if (!file.is_open()) return false;

	file.seekg(0, file.end);
	size_t size = (size_t)file.tellg();
	file.seekg(0, file.beg);

	_shaderInfo.fragmentSource.resize(size);
	file.read(_shaderInfo.fragmentSource.data(), size);

	return true;
}

bool Shader::build_shaders() {
	auto vertexSource = _shaderInfo.vertexSource.data();
	auto fragmentSource = _shaderInfo.fragmentSource.data();

	glShaderSource(_shaderInfo.vertexId, 1, &vertexSource, nullptr);
	glCompileShader(_shaderInfo.vertexId);
	
	if (check_shader_error(_shaderInfo.vertexId)) return false;

	glShaderSource(_shaderInfo.fragmentId, 1, &fragmentSource, nullptr);
	glCompileShader(_shaderInfo.fragmentId);

	if (check_shader_error(_shaderInfo.fragmentId)) return false;

	_shaderInfo.programId = glCreateProgram();
	glAttachShader(_shaderInfo.programId, _shaderInfo.vertexId);
	glAttachShader(_shaderInfo.programId, _shaderInfo.fragmentId);
	glLinkProgram(_shaderInfo.programId);

	glDeleteShader(_shaderInfo.vertexId);
	glDeleteShader(_shaderInfo.fragmentId);

	return true;
}

void Shader::use() const {
	glUseProgram(_shaderInfo.programId);
}

bool Shader::check_shader_error(u32 shader) {
	i32 success = 0;
	std::string log(512, 0);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, nullptr, log.data());
		std::cout << "Error compilation shader" << std::endl << log << std::endl;
		return true;
	}
	return false;
}

bool Shader::check_program_error(u32 program) {
	i32 success = 0;
	std::string log(512, 0);
	glGetProgramiv(program, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, nullptr, log.data());
		std::cout << "Error compilation shader" << std::endl << log << std::endl;
		return true;
	}
	return false;
}

void Shader::set_uni_4f(const std::string& name, Vector4f uni) const {
	use();
	glUniform4f(
		glGetUniformLocation(_shaderInfo.programId, name.c_str()),
		XYZW_UNROLL(uni)
	);
}

void Shader::set_uni_mat4f(
	const std::string& name, const Matrix4f& mat
) const {
	use();
	glUniformMatrix4fv(
		glGetUniformLocation(_shaderInfo.programId, name.c_str()),
		1,
		GL_TRUE,
		&(mat.rows[0].x)
	);
}

void Shader::set_uni_mat4f(const std::string& name, const float* mat) const {
	use();
	glUniformMatrix4fv(
		glGetUniformLocation(_shaderInfo.programId, name.c_str()),
		1,
		GL_TRUE,
		mat
	);
}
