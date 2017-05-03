#pragma once

#include <vector>

#include <GL\glew.h>
#include <glm\glm.hpp>

#include "Structures.h"

namespace Loader
{
	GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
	VBO* loadOBJ(const char * path);
	void indexVBO(std::vector<glm::vec3> & in_vertices, std::vector<glm::vec2> & in_uvs, std::vector<glm::vec3> & in_normals, std::vector<unsigned short> & out_indices, std::vector<glm::vec3> & out_vertices, std::vector<glm::vec2> & out_uvs, std::vector<glm::vec3> & out_normals);
}
