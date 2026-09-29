// gl/ObjLoader.h
#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

bool loadObj(
    const std::string& path,
	std::vector<glm::vec3>& outPositions, std::vector<glm::vec3>& outNormals);