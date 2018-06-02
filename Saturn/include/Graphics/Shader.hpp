#pragma once

#include <string>

#include "Common.hpp"
#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"

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

	bool load_vertex(const std::string& path);
	bool load_fragment(const std::string& path);

	bool build_shaders();

	void use() const;

	void set_uni_4f(const std::string& name, Vector4f uni) const;
	void set_uni_mat4f(const std::string& name, const Matrix4f& mat) const;
	void set_uni_mat4f(const std::string& name, const float* mat) const;

private:

	bool check_shader_error(u32 shader);
	bool check_program_error(u32 program);

	ShaderInfo _shaderInfo;
};