#include <iostream>
#include <sstream>
#include <fstream>

#include "Graphics/Shader.hpp"

#include <glad/glad.h>

Shader::Shader() {
	_info.vertexId = glCreateShader(GL_VERTEX_SHADER);
	_info.fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
}

Shader::~Shader() {
	glDeleteShader(_info.vertexId);
	glDeleteShader(_info.fragmentId);
	glDeleteProgram(_info.programId);
}


Shader::Shader(const Shader&& that) : _info(that._info) {}
Shader& Shader::operator=(const Shader&& that) {
	_info = that._info;
	return *this;
}

void Shader::load_vertex(const std::string& path) {
	std::ifstream file(path);

	_ASSERT_EXPR(file.is_open(), ("Can't find shader: " + path).c_str());

	file.seekg(0, file.end);
	size_t size = (size_t)file.tellg();
	file.seekg(0, file.beg);

	_info.vertexSource.resize(size);
	file.read(_info.vertexSource.data(), size);
}
void Shader::load_fragment(const std::string& path) {
	std::ifstream file(path);

	_ASSERT_EXPR(file.is_open(), ("Can't find shader: " + path).c_str());

	file.seekg(0, file.end);
	size_t size = (size_t)file.tellg();
	file.seekg(0, file.beg);

	_info.fragmentSource.resize(size);
	file.read(_info.fragmentSource.data(), size);
}

void Shader::build_shaders() {
	auto vertexSource = _info.vertexSource.data();
	auto fragmentSource = _info.fragmentSource.data();

	glShaderSource(_info.vertexId, 1, &vertexSource, nullptr);
	glCompileShader(_info.vertexId);
	
	check_shader_error(_info.vertexId);

	glShaderSource(_info.fragmentId, 1, &fragmentSource, nullptr);
	glCompileShader(_info.fragmentId);

	check_shader_error(_info.fragmentId);

	_info.programId = glCreateProgram();
	glAttachShader(_info.programId, _info.vertexId);
	glAttachShader(_info.programId, _info.fragmentId);
	glLinkProgram(_info.programId);

	glDeleteShader(_info.vertexId);
	glDeleteShader(_info.fragmentId);
}

void Shader::use() const {
	glUseProgram(_info.programId);
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

void Shader::set_uni_4f(const std::string& name, Vector4f uni) const {
	use();
	glUniform4f(
		glGetUniformLocation(_info.programId, name.c_str()),
		XYZW_UNROLL(uni)
	);
}

void Shader::set_uni_mat4f(
	const std::string& name, const Matrix4f& mat
) const {
	use();
	glUniformMatrix4fv(
		glGetUniformLocation(_info.programId, name.c_str()),
		1,
		GL_TRUE,
		&(mat.rows[0].x)
	);
}

void Shader::set_uni_mat4f(const std::string& name, const float* mat) const {
	use();
	glUniformMatrix4fv(
		glGetUniformLocation(_info.programId, name.c_str()),
		1,
		GL_TRUE,
		mat
	);
}
