#pragma once

#include <vector>
#include <string>

#include <GL\glew.h>
#include <glm\glm.hpp>

class DirectionalLight;
class PointLight;

const float PI = 3.14159265359f;

enum ObjectType {
	DYNAMIC_TEXTURED_OBJECT,
	DYNAMIC_MATERIAL_OBJECT,
	STATIC_TEXTURED_OBJECT,
	STATIC_MATERIAL_OBJECT,
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	ROOT
};

enum GameState {
	MapEditorState,
	GamePlayState
};

struct BasicShader {
	GLuint shaderID, matrixID, viewMatrixID;
};

struct LightingShader : BasicShader
{
	GLuint dirLightPositionID, dirLightAmbientID, dirLightDiffuseID, dirLightSpecularID;
	GLuint pointLightPositionID[4], pointLightAmbientID[4], pointLightDiffuseID[4], pointLightSpecularID[4], pointLightConstantID[4], pointLightLinearID[4], pointLightQuardraticID[4];
	GLuint viewPosID;
};

struct Material
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	GLfloat shininess;
};

struct VBO
{
	std::string *path;
	bool indexed, containsUVs;
	GLuint vertexArrayID;
	GLuint uvArrayID;
	GLuint normalArrayID;
	GLuint elementArrayID;
	unsigned int indicesSize;
	float minX, maxX, minY, maxY, minZ, maxZ;
	Material* material;
};

namespace MatrixHolder
{
	extern glm::mat4 viewMatrix;
	extern glm::mat4 projectionMatrix;
};

namespace ShaderManager
{
	extern LightingShader materialShader;
	extern LightingShader textureShader;
	extern BasicShader billboardShader;
}

namespace LightManager
{
	extern DirectionalLight* directionalLight;
	extern PointLight* pointLights[4];
}

namespace GameStateManager
{
	extern GameState gameState;
	extern float deltaTime;
}

namespace FileManager
{
	extern std::string currentFilePath;
}

struct PackedVertex
{
	glm::vec3 vertex;
	glm::vec3 normal;
	glm::vec2 uv;
	bool operator<(const PackedVertex that) const {
		return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
	};
};

struct DirLight
{
	glm::vec3 ambient, diffuse, specular;
};

struct PntLight : DirLight
{
	float constant;
	float linear;
	float quadratic;
};

struct Camera
{
	Camera(glm::vec3 positionIN, float FoVIN, float speedIN) {
		position = positionIN;
		FoV = FoVIN;
		speed = speedIN;
		horizontalAngle = PI/2;
		verticalAngle = -0.4f;
	}

	void ScreenPositionToWorldRay(float mouseX, float mouseY, int screenWidth, int screenHeight, glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::vec3 &out_origin, glm::vec3 &out_direction)
	{
		glm::vec4 rayStart(((float)mouseX / (float)screenWidth - 0.5f) * 2.0f, ((float)(screenHeight-mouseY)/ (float)screenHeight - 0.5f) *2.0f, -1.0, 1.0f);

		glm::vec4 rayEnd(((float)mouseX / (float)screenWidth - 0.5f) *2.0f, ((float)(screenHeight - mouseY) / (float)screenHeight - 0.5f) *2.0f, 0.0, 1.0f);

		glm::mat4 inverseProjectionMatrix = glm::inverse(projectionMatrix);
		glm::mat4 inverseViewMatrix = glm::inverse(viewMatrix);

		glm::mat4 matrix = glm::inverse(projectionMatrix * viewMatrix);
		glm::vec4 rayStartWorld = matrix * rayStart;
		rayStartWorld /= rayStartWorld.w;
		glm::vec4 rayEndWorld = matrix * rayEnd;
		rayEndWorld /= rayEndWorld.w;

		glm::vec3 rayDirectionWorld(rayEndWorld - rayStartWorld);
		rayDirectionWorld = glm::normalize(rayDirectionWorld);

		out_origin = glm::vec3(rayStartWorld);
		out_direction = glm::normalize(rayDirectionWorld);
	}

	bool TestRayIntersection(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 minPosition, glm::vec3 maxPosition, glm::mat4 modelMatrix, float &intersectionDistnace)
	{
		float intersectionMin = 0.0f;
		float intersectionMax = 100000.0f;

		glm::vec3 ObjectBoundingBoxWorldSpace(modelMatrix[3].x, modelMatrix[3].y, modelMatrix[3].z);

		glm::vec3 delta = ObjectBoundingBoxWorldSpace - rayOrigin;


		// Test xAxis
		{
			glm::vec3 xAxis(modelMatrix[0].x, modelMatrix[0].y, modelMatrix[0].z);
			float i = glm::dot(xAxis, delta);
			float j = glm::dot(rayDirection, xAxis);

			if (fabs(j) > 0.001f) {
				float intersect1 = (i + minPosition.x) / j;
				float intersect2 = (i + maxPosition.x) / j;

				// Make sure intersect1 is the near intersection
				if (intersect1 > intersect2)
				{
					float temp = intersect1;
					intersect1 = intersect2;
					intersect2 = temp;
				}

				if (intersect2 < intersectionMax)
					intersectionMax = intersect2;
				if (intersect1 > intersectionMin)
					intersectionMin = intersect1;

				if (intersectionMin > intersectionMax)
					return false;
			}
			else if (-i + minPosition.x > 0.0f || -i + maxPosition.x < 0.0f) // If ray is almost parallel to the plane, there are no intersections
			{
				return false;
			}
		}

		// Repeat for yAxis
		{
			glm::vec3 yAxis(modelMatrix[1].x, modelMatrix[1].y, modelMatrix[1].z);
			float i = glm::dot(yAxis, delta);
			float j = glm::dot(rayDirection, yAxis);

			if (fabs(j) > 0.001f) {
				float intersect1 = (i + minPosition.y) / j;
				float intersect2 = (i + maxPosition.y) / j;

				if (intersect1 > intersect2)
				{
					float temp = intersect1;
					intersect1 = intersect2;
					intersect2 = temp;
				}

				if (intersect2 < intersectionMax)
					intersectionMax = intersect2;
				if (intersect1 > intersectionMin)
					intersectionMin = intersect1;

				if (intersectionMin > intersectionMax)
					return false;
			}
			else if (-i + minPosition.y > 0.0f || -i + maxPosition.y < 0.0f)
			{
				return false;
			}
		}

		// Repeat for zAxis
		{
			glm::vec3 zAxis(modelMatrix[2].x, modelMatrix[2].y, modelMatrix[2].z);
			float i = glm::dot(zAxis, delta);
			float j = glm::dot(rayDirection, zAxis);

			if (fabs(j) > 0.001f) {
				float intersect1 = (i + minPosition.z) / j;
				float intersect2 = (i + maxPosition.z) / j;

				if (intersect1 > intersect2)
				{
					float temp = intersect1;
					intersect1 = intersect2;
					intersect2 = temp;
				}

				if (intersect2 < intersectionMax)
					intersectionMax = intersect2;
				if (intersect1 > intersectionMin)
					intersectionMin = intersect1;
				
				if (intersectionMin > intersectionMax)
					return false;
			}
			else if (-i + minPosition.z > 0.0f || -i + maxPosition.z < 0.0f)
			{
				return false;
			}
		}

		intersectionDistnace = intersectionMin;
		return true;
	}

	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 right;
	glm::vec3 up;
	float horizontalAngle;
	float verticalAngle;
	float FoV;
	float speed;
};