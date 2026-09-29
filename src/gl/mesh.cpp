#include "gl/mesh.h"
#include "gl/gl_check.h"
void Mesh::upload(const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& normals)
{
	vertexCount_ = static_cast<int>(positions.size());

	GL_CHECK(glGenVertexArrays(1, &vao_));
	GL_CHECK(glBindVertexArray(vao_));

	GLuint buffers[2];
	GL_CHECK(glGenBuffers(2, buffers));
	vboPos_ = buffers[0];
	vboNormal_ = buffers[1];

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vboPos_));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STATIC_DRAW));

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vboNormal_));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW));
}

Mesh::~Mesh()
{
}

void Mesh::draw(const Shader& shader) const
{
	GLint aPos = shader.attribute("a_position");
	GLint aNormal = shader.attribute("a_normal");

	GL_CHECK(glBindVertexArray(vao_));

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vboPos_));
	GL_CHECK(glEnableVertexAttribArray(aPos));
	GL_CHECK(glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, 0, nullptr));

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vboNormal_));
	GL_CHECK(glEnableVertexAttribArray(aNormal));
	GL_CHECK(glVertexAttribPointer(aNormal, 3, GL_FLOAT, GL_FALSE, 0, nullptr));

	GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, vertexCount_));
}