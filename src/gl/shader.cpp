#include "gl/shader.h"
#include "gl/gl_check.h"
#include <SDL3/SDL.h>
#include <fstream>
#include <sstream>
#include <regex>
#include <vector>
namespace
{
	bool readFile(const std::string& path, std::string& out)
	{
		std::ifstream file(path);
		if (!file)
		{
			SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Shader: could not open %s", path.c_str());
			return false;
		}
		std::stringstream ss;
		ss << file.rdbuf();
		out = ss.str();
		return true;
	}

	void scanDeclarations(const std::string& source, std::vector<std::string>& attribs,
		std::vector<std::string>& uniforms)
	{
		static const std::regex attribRe(R"((?:^|\n)\s*(?:in|attribute)\s+\w+\s+(\w+)\s*;)");
		static const std::regex uniformRe(R"((?:^|\n)\s*uniform\s+\w+\s+(\w+)\s*;)");
		for (auto it = std::sregex_iterator(source.begin(), source.end(), attribRe);
			it != std::sregex_iterator(); ++it)
		{
			attribs.push_back((*it)[1].str());
		}
		for (auto it = std::sregex_iterator(source.begin(), source.end(), uniformRe);
			it != std::sregex_iterator(); ++it)
		{
			uniforms.push_back((*it)[1].str());
		}
	}
}

GLuint Shader::compile(GLenum type, const std::string& source, const std::string& debugName)
{
	GLuint shader = glCreateShader(type);
	const char* src = source.c_str();
	GLint len = static_cast<GLint>(source.size());
	glShaderSource(shader, 1, &src, &len);
	glCompileShader(shader);

	GLint status = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint logLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
		std::string log(logLen, '\0');
		glGetShaderInfoLog(shader, logLen, nullptr, log.data());
		SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Shader compile failed (%s):\n%s",
			debugName.c_str(), log.c_str());
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

Shader Shader::loadFromFiles(const std::string& basePath)
{
	Shader result;

	std::string vertSrc, fragSrc;
	if (!readFile(basePath + ".vert", vertSrc)) return result;
	if (!readFile(basePath + ".frag", fragSrc)) return result;

	GLuint vert = compile(GL_VERTEX_SHADER, vertSrc, basePath + ".vert");
	GLuint frag = vert ? compile(GL_FRAGMENT_SHADER, fragSrc, basePath + ".frag") : 0;
	if (!vert || !frag)
	{
		if (vert) glDeleteShader(vert);
		if (frag) glDeleteShader(frag);
		return result; // invalid — id() == 0
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vert);
	glAttachShader(program, frag);
	glLinkProgram(program);
	glDeleteShader(vert);
	glDeleteShader(frag);

	GLint linkStatus = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE)
	{
		GLint logLen = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
		std::string log(logLen, '\0');
		glGetProgramInfoLog(program, logLen, nullptr, log.data());
		SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Shader link failed (%s):\n%s",
			basePath.c_str(), log.c_str());
		return result; // invalid
	}

	result.program_ = program;
	result.reflect(vertSrc, fragSrc);
	return result;
}

void Shader::reflect(const std::string& vertSource, const std::string& fragSource)
{
	std::vector<std::string> attribs, vertUniforms, fragUniforms;
	scanDeclarations(vertSource, attribs, vertUniforms);
	std::vector<std::string> dummy;
	scanDeclarations(fragSource, dummy, fragUniforms);

	for (const auto& name : attribs)
		attributes_[name] = glGetAttribLocation(program_, name.c_str());
	for (const auto& name : vertUniforms)
		uniforms_[name] = glGetUniformLocation(program_, name.c_str());
	for (const auto& name : fragUniforms)
		uniforms_[name] = glGetUniformLocation(program_, name.c_str());
}

Shader::~Shader()
{
	// program_ == 0 is a harmless no-op for glDeleteProgram; safe on a
	// default-constructed or moved-from Shader.
}

Shader::Shader(Shader&& other) noexcept
	: program_(other.program_)
	, attributes_(std::move(other.attributes_))
	, uniforms_(std::move(other.uniforms_))
{
	other.program_ = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
	if (this != &other)
	{
		program_ = other.program_;
		attributes_ = std::move(other.attributes_);
		uniforms_ = std::move(other.uniforms_);
		other.program_ = 0;
	}
	return *this;
}

void Shader::use() const
{
	glUseProgram(program_);
}

GLint Shader::attribute(const std::string& name) const
{
	auto it = attributes_.find(name);
	if (it == attributes_.end())
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "Shader: unknown attribute '%s'", name.c_str());
		return -1;
	}
	return it->second;
}

GLint Shader::uniform(const std::string& name) const
{
	auto it = uniforms_.find(name);
	if (it == uniforms_.end())
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "Shader: unknown uniform '%s'", name.c_str());
		return -1;
	}
	return it->second;
}
