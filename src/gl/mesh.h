#pragma once
#include "gl/gl.h"
#include "gl/shader.h"
#include <vector>
#include <glm/glm.hpp>


class Mesh
{
public:
	void upload(const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& normals);
	~Mesh();
	Mesh() = default;
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	void draw(const Shader& shader) const;

private:
	GLuint vao_ = 0;
	GLuint vboPos_ = 0, vboNormal_ = 0;
	int vertexCount_ = 0;
};
