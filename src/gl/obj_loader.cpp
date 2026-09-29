#include "gl/obj_loader.h"
#include <fstream>
#include <sstream>

bool loadObj(const std::string& path,
    std::vector<glm::vec3>& outPositions, std::vector<glm::vec3>& outNormals)
{
    std::ifstream file(path);
    if (!file.is_open()) return false;

    std::vector<glm::vec3> positions, normals;
    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string tag;
        ss >> tag;

        if (tag == "v")
        {
            glm::vec3 p;
            ss >> p.x >> p.y >> p.z;
            positions.push_back(p);
        }
        else if (tag == "vn")
        {
            glm::vec3 n;
            ss >> n.x >> n.y >> n.z;
            normals.push_back(n);
        }
        else if (tag == "f")
        {
            std::vector<std::pair<int, int>> verts;
            std::string token;
            while (ss >> token)
            {
                int vi = 0, ni = 0;
                size_t first = token.find('/');
                size_t last = token.rfind('/');
                vi = std::stoi(token.substr(0, first));
                if (last != std::string::npos && last + 1 < token.size())
                    ni = std::stoi(token.substr(last + 1));
                verts.push_back({vi, ni});
            }
            for (size_t i = 1; i + 1 < verts.size(); ++i)
            {
                int idx[3] = {0, static_cast<int>(i), static_cast<int>(i + 1)};
                for (int k : idx)
                {
                    auto [vi, ni] = verts[k];
                    outPositions.push_back(positions[vi - 1]); // OBJ indices are 1-based
                    outNormals.push_back(ni > 0 ? normals[ni - 1] : glm::vec3(0, 1, 0));
                }
            }
        }
    }
    return !outPositions.empty();
}