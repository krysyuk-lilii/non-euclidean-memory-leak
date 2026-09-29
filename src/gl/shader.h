#pragma once
#include "gl/gl.h"
#include <string>
#include <unordered_map>
class Shader
{
public:
	static Shader loadFromFiles(const std::string& basePath);

	Shader() = default;
	~Shader();
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader(Shader&& other) noexcept;
	Shader& operator=(Shader&& other) noexcept;

	bool valid() const { return program_ != 0; }
	void use() const;

	GLint attribute(const std::string& name) const;
	GLint uniform(const std::string& name) const;

private:
	GLuint program_ = 0;
	std::unordered_map<std::string, GLint> attributes_;
	std::unordered_map<std::string, GLint> uniforms_;

	static GLuint compile(GLenum type, const std::string& source, const std::string& debugName);
	void reflect(const std::string& vertSource, const std::string& fragSource);
};
