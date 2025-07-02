#ifndef FISH_H
#define FISH_H

#include "headers.h"
#include "imagetexture.h"
#include "shaderprog.h"
#include "material.h"
#include "camera.h"

// VertexPT Declarations.
struct VertexPTN_
{
	VertexPTN_() {
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		normal = glm::vec3(0.0f, 1.0f, 0.0f);
		texcoord = glm::vec2(0.0f, 0.0f);
	}
	VertexPTN_(glm::vec3 p, glm::vec3 n, glm::vec2 uv) {
		position = p;
		normal = n;
		texcoord = uv;
	}
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;
};


struct VertexPTNHash_ {
	size_t operator()(const VertexPTN_& vertex) const {
		return std::hash<float>()(vertex.position.x) ^
			std::hash<float>()(vertex.position.y) ^
			std::hash<float>()(vertex.position.z) ^
			std::hash<float>()(vertex.normal.x) ^
			std::hash<float>()(vertex.normal.y) ^
			std::hash<float>()(vertex.normal.z) ^
			std::hash<float>()(vertex.texcoord.x) ^
			std::hash<float>()(vertex.texcoord.y);
	}
};



struct VertexPTNEquals_ {
	bool operator()(const VertexPTN_& lhs, const VertexPTN_& rhs) const {
		return lhs.position == rhs.position &&
			lhs.normal == rhs.normal &&
			lhs.texcoord == rhs.texcoord;
	}
};


struct SubMesh_
{
	SubMesh_() {
		material = nullptr;
		iboId = 0;
	}
	PhongMaterial* material;
	GLuint iboId;
	std::vector<unsigned int> vertexIndices;
};


class Fish {
public:
	Fish(float ra, float scale,glm::mat4 roo);
	~Fish();
	void Render(Camera* camera, PhongShadingDemoShaderProg* shader);
	void Update();

	bool LoadFromFile(const std::string& filePath, const bool normalized = true);
	void parseMTL(const std::string& filename);
	void CreateBuffer();
	int GetVertexIndexHash(const VertexPTN_& vertex);

	int GetNumVertices() const { return numVertices; }
	int GetNumTriangles() const { return numTriangles; }
	int GetNumSubMeshes() const { return (int)subMeshes.size(); }
	glm::vec3 GetObjCenter() const { return objCenter; }
	glm::vec3 GetObjExtent() const { return objExtent; }

	std::unordered_map<VertexPTN_, int, VertexPTNHash_, VertexPTNEquals_> vertexMap;

private:

	float radius;
	float scale;
	glm::mat4 ObjRotationMatrix;
	

	GLuint vboId, instanceVBO;
	std::vector<VertexPTN_> vertices;
	std::vector<glm::mat4> instanceMatrices;

	std::vector<glm::vec3> v_data;
	std::vector<glm::vec3> vn_data;
	std::vector<glm::vec2> vt_data;

	std::vector<SubMesh_> subMeshes;

	std::unordered_map<std::string, PhongMaterial*> mtl;

	int numVertices;

	int numTriangles;

	glm::vec3 objCenter;

	glm::vec3 objExtent;
};

#endif

