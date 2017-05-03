#include "Loader.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <map>

namespace Loader
{

	GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path) {

		// Create the shaders
		GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

		// Read the Vertex Shader code from the file
		std::string VertexShaderCode;
		std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
		if (VertexShaderStream.is_open()) {
			std::string Line = "";
			while (getline(VertexShaderStream, Line))
				VertexShaderCode += "\n" + Line;
			VertexShaderStream.close();
		}
		else {
			printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
			return 0;
		}

		// Read the Fragment Shader code from the file
		std::string FragmentShaderCode;
		std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
		if (FragmentShaderStream.is_open()) {
			std::string Line = "";
			while (getline(FragmentShaderStream, Line))
				FragmentShaderCode += "\n" + Line;
			FragmentShaderStream.close();
		}

		GLint Result = GL_FALSE;
		int InfoLogLength;


		// Compile Vertex Shader
		printf("Compiling shader : %s\n", vertex_file_path);
		char const * VertexSourcePointer = VertexShaderCode.c_str();
		glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
		glCompileShader(VertexShaderID);

		// Check Vertex Shader
		glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
			printf("%s\n", &VertexShaderErrorMessage[0]);
		}



		// Compile Fragment Shader
		printf("Compiling shader : %s\n", fragment_file_path);
		char const * FragmentSourcePointer = FragmentShaderCode.c_str();
		glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
		glCompileShader(FragmentShaderID);

		// Check Fragment Shader
		glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
			printf("%s\n", &FragmentShaderErrorMessage[0]);
		}



		// Link the program
		printf("Linking program\n");
		GLuint ProgramID = glCreateProgram();
		glAttachShader(ProgramID, VertexShaderID);
		glAttachShader(ProgramID, FragmentShaderID);
		glLinkProgram(ProgramID);

		// Check the program
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			printf("%s\n", &ProgramErrorMessage[0]);
		}


		glDetachShader(ProgramID, VertexShaderID);
		glDetachShader(ProgramID, FragmentShaderID);

		glDeleteShader(VertexShaderID);
		glDeleteShader(FragmentShaderID);

		return ProgramID;
	}

	VBO* loadOBJ(const char * path)
	{
		printf("Loading OBJ file %s...\n", path);

		std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
		std::vector<glm::vec3> temp_vertices, final_vertices;
		std::vector<glm::vec2> temp_uvs, final_uvs;
		std::vector<glm::vec3> temp_normals, final_normals;
		float maxX = 0, minX = 0, maxY = 0, minY = 0, maxZ = 0, minZ = 0;


		std::ifstream file;
		file.open(path, std::ios::in);
		if (!file.good()) {
			printf("Impossible to open the file %s!\n", path);
			return false;
		}

		bool containsUVs = true;
		bool containsMaterial = false;
		std::string materialPath = "";

		std::string line;

		while (getline(file, line)) {

			char lineHeader[128];
			const char* cLine = line.c_str();
			std::stringstream ssLine(line);

			// read the first word of the line
			ssLine >> lineHeader;

			if (strcmp(lineHeader, "v") == 0) {
				glm::vec3 vertex;
				std::string temp;
				ssLine >> vertex.x >> vertex.y >> vertex.z;
				if (vertex.x > maxX) maxX = vertex.x;
				if (vertex.x < minX) minX = vertex.x;
				if (vertex.y > maxY) maxY = vertex.y;
				if (vertex.y < minY) minY = vertex.y;
				if (vertex.z > maxZ) maxZ = vertex.z;
				if (vertex.z < minZ) minZ = vertex.z;
				temp_vertices.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0) {
				glm::vec2 uv;
				ssLine >> uv.x >> uv.y;
				temp_uvs.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0) {
				glm::vec3 normal;
				ssLine >> normal.x >> normal.y >> normal.z;
				temp_normals.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) {
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				if (sscanf_s(cLine, "f %d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]) == 9) {
					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[1]);
					vertexIndices.push_back(vertexIndex[2]);
					uvIndices.push_back(uvIndex[0]);
					uvIndices.push_back(uvIndex[1]);
					uvIndices.push_back(uvIndex[2]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);
					containsUVs = true;
				}
				else if(sscanf_s(cLine, "f %d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]) == 6)
				{					
					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[1]);
					vertexIndices.push_back(vertexIndex[2]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);
					containsUVs = false;
				}
				else 
				{
					printf("Obj files are required to be in triangles and contain normals.");
					return false;
				}
			}
			else if (strcmp(lineHeader, "mtllib") == 0){
				ssLine >> materialPath;
				containsMaterial = true;
			}
		}

		// For each vertex of each triangle
		for (unsigned int i = 0; i<vertexIndices.size(); i++) {

			// Get the indices of its attributes
			unsigned int vertexIndex = vertexIndices[i];
			unsigned int normalIndex = normalIndices[i];

			// Get the attributes thanks to the index
			glm::vec3 vertex = temp_vertices[vertexIndex - 1];
			glm::vec3 normal = temp_normals[normalIndex - 1];

			// Put the attributes in buffers
			final_vertices.push_back(vertex);
			final_normals.push_back(normal);

			if (containsUVs) {
				unsigned int uvIndex = uvIndices[i];
				glm::vec2 uv = temp_uvs[uvIndex - 1];
				final_uvs.push_back(uv);
			}

		}

		VBO* vbo;

		if (containsUVs) {
			std::vector<glm::vec3> indexed_vertices;
			std::vector<glm::vec3> indexed_normals;
			std::vector<glm::vec2> indexed_uvs;
			std::vector<unsigned short> indices;

			Loader::indexVBO(final_vertices, final_uvs, final_normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

			vbo = new VBO();
			
			glGenBuffers(1, &vbo->vertexArrayID);
			glBindBuffer(GL_ARRAY_BUFFER, vbo->vertexArrayID);
			glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

			glGenBuffers(1, &vbo->uvArrayID);
			glBindBuffer(GL_ARRAY_BUFFER, vbo->uvArrayID);
			glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

			glGenBuffers(1, &vbo->normalArrayID);
			glBindBuffer(GL_ARRAY_BUFFER, vbo->normalArrayID);
			glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

			glGenBuffers(1, &vbo->elementArrayID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo->elementArrayID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);

			vbo->indicesSize = (unsigned int)indices.size();
			vbo->indexed = true;
			vbo->containsUVs = true;
		}
		else {
			vbo = new VBO;

			glGenBuffers(1, &vbo->vertexArrayID);
			glBindBuffer(GL_ARRAY_BUFFER, vbo->vertexArrayID);
			glBufferData(GL_ARRAY_BUFFER, final_vertices.size() * sizeof(glm::vec3), &final_vertices[0], GL_STATIC_DRAW);

			glGenBuffers(1, &vbo->normalArrayID);
			glBindBuffer(GL_ARRAY_BUFFER, vbo->normalArrayID);
			glBufferData(GL_ARRAY_BUFFER, final_normals.size() * sizeof(glm::vec3), &final_normals[0], GL_STATIC_DRAW);

			vbo->indicesSize = (unsigned int)final_vertices.size();
			vbo->indexed = false;
			vbo->containsUVs = false;
		}

		vbo->maxX = maxX;
		vbo->minX = minX;
		vbo->maxY = maxY;
		vbo->minY = minY;
		vbo->maxZ = maxZ;
		vbo->minZ = minZ;

		Material* mat = new Material();

		if (containsMaterial)
		{
			std::ifstream file;
			file.open("materials/models/" + materialPath);
			if (file.good())
			{
				printf("Loading material: materials/models/%s\n", materialPath.c_str());
				while (getline(file, line))
				{
					char lineHeader[128];

					const char* cLine = line.c_str();
					
					std::stringstream ssLine(line);

					ssLine >> lineHeader;

					if (strcmp(lineHeader, "Ka") == 0)
					{
						ssLine >> mat->ambient.x >> mat->ambient.y >> mat->ambient.z;
					}
					else if (strcmp(lineHeader, "Kd") == 0)
					{
						ssLine >> mat->diffuse.x >> mat->diffuse.y >> mat->diffuse.z;
					}
					else if (strcmp(lineHeader, "Ks") == 0)
					{
						ssLine >> mat->specular.x >> mat->specular.y >> mat->specular.z;
					}
					else if (strcmp(lineHeader, "Ns") == 0)
					{
						ssLine >> mat->shininess;
					}
				}
			} else {
				printf("Couldn't open material: materials/models/%s\n", materialPath.c_str());
				containsMaterial = false;
				mat->ambient = glm::vec3(0.1f, 0.1f, 0.1f);
				mat->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
				mat->specular = glm::vec3(0.5f, 0.5f, 0.5f);
				mat->shininess = 32;
			}
			
		} else {
			printf("No material specified, using default material...\n");
			mat->ambient = glm::vec3(0.1f, 0.1f, 0.1f);
			mat->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
			mat->specular = glm::vec3(0.5f, 0.5f, 0.5f);
			mat->shininess = 32;
		}

		vbo->material = mat;

		return vbo;
	}

	bool FindSimilarVertexIndex(PackedVertex & packed, std::map<PackedVertex, unsigned short> & VertexToOutIndex, unsigned short & result) {
		std::map<PackedVertex, unsigned short>::iterator it = VertexToOutIndex.find(packed);
		if (it == VertexToOutIndex.end()) {
			return false;
		}
		else {
			result = it->second;
			return true;
		}
	}

	void indexVBO(std::vector<glm::vec3> & in_vertices,	std::vector<glm::vec2> & in_uvs, std::vector<glm::vec3> & in_normals, std::vector<unsigned short> & out_indices, std::vector<glm::vec3> & out_vertices,	std::vector<glm::vec2> & out_uvs, std::vector<glm::vec3> & out_normals) {
		

		std::map<PackedVertex, unsigned short> VertexToOutIndex;

		// For each input vertex
		for (unsigned int i = 0; i<in_vertices.size(); i++) {

			PackedVertex packed = { in_vertices[i], in_normals[i], in_uvs[i] };


			// Try to find a similar vertex in out_XXXX
			unsigned short index;
			bool found = FindSimilarVertexIndex(packed, VertexToOutIndex, index);

			if (found) { // A similar vertex is already in the VBO, use it instead !
				out_indices.push_back(index);
			}
			else { // If not, it needs to be added in the output data.
				out_vertices.push_back(in_vertices[i]);
				out_uvs.push_back(in_uvs[i]);
				out_normals.push_back(in_normals[i]);
				unsigned short newindex = (unsigned short)out_vertices.size() - 1;
				out_indices.push_back(newindex);
				VertexToOutIndex[packed] = newindex;
			}
		}
	}
}