#ifndef LINE_H
#define LINE_H

#include "headers.h"
#include "imagetexture.h"
#include "shaderprog.h"
#include "material.h"
#include "camera.h"


// VertexPT Declarations.
struct VertexPT_
{
	VertexPT_() {
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		texcoord = glm::vec2(0.0f, 0.0f);
	}
	VertexPT_(glm::vec3 p, glm::vec2 uv) {
		position = p;
		texcoord = uv;
	}
	glm::vec3 position;
	glm::vec2 texcoord;
};


class Line
{
public:
	// Line Public Methods.
	Line(const std::string& texImagePath);
	~Line();
	void Render(Camera* camera, LineShaderProg* shader);

	void SetRotation(const float newRotation) { rotationY = newRotation; }

	void SetWorldMatrix(const glm::mat4x4 NewWorldMatrix) { WorldMatrix = NewWorldMatrix; }

	ImageTexture* GetTexture() { return panorama; };
	float GetRotation() const { return rotationY; }

private:
	// Line Private Methods.
	static void CreateSquare(std::vector<VertexPT_>& vertices, std::vector<unsigned int>& indices);

	// Line Private Data.
	GLuint vboId;
	GLuint iboId;
	std::vector<VertexPT_> vertices;
	std::vector<unsigned int> indices;

	LineMaterial* material;
	ImageTexture* panorama;

	glm::mat4x4 WorldMatrix;

	float rotationY;
};

#endif

