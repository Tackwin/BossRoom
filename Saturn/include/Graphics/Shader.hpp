#pragma once

#include <string>

#include "Common.hpp"

struct ShaderInfo {
	u32 vertexId = 0u;
	u32 programId = 0u;
	u32 fragmentId = 0u;

	std::string vertexSource = "";
	std::string fragmentSource = "";
};

class Shader {
public:

	Shader();
	~Shader();

	void load_vertex(const std::string& path);
	void load_fragment(const std::string& path);

	void build_shaders();

	void use() const;

private:

	void check_shader_error(u32 shader);
	void check_program_error(u32 program);

	ShaderInfo _shaderInfo;
};