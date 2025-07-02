#pragma once
#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include <unordered_map>
#include "headers.h"
#include "material.h"

// VertexPTN Declarations.
struct VertexPTN
{
	VertexPTN() {
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		normal = glm::vec3(0.0f, 1.0f, 0.0f);
		texcoord = glm::vec2(0.0f, 0.0f);
	}
	VertexPTN(glm::vec3 p, glm::vec3 n, glm::vec2 uv) {
		position = p;
		normal = n;
		texcoord = uv;
	}
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;
};

//Function Object for Calculate Hash Value
struct VertexPTNHash {
	size_t operator()(const VertexPTN& vertex) const {
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

//Function Object for Equals
struct VertexPTNEquals {
	bool operator()(const VertexPTN& lhs, const VertexPTN& rhs) const {
		return lhs.position == rhs.position &&
			lhs.normal == rhs.normal &&
			lhs.texcoord == rhs.texcoord;
	}
};

// SubMesh Declarations.
struct SubMesh
{
	SubMesh() {
		material = nullptr;
		iboId = 0;
	}
	PhongMaterial* material;
	GLuint iboId;
	std::vector<unsigned int> vertexIndices;
};


// TriangleMesh Declarations.
class TriangleMesh
{
public:
	// TriangleMesh Public Methods.
	TriangleMesh();
	~TriangleMesh();

	// Load the model from an *.OBJ file.
	bool LoadFromFile(const std::string& filePath, const bool normalized = true);

	// Show model information.
	void ShowInfo();

	// Process .mtl
	void parseMTL(const std::string& filename);

	// Show .mtl Info
	void ShowMTL();

	// Create Buffer like HW1
	void CreateBuffer();

	// Render
	void Render(PhongShadingDemoShaderProg* shader);

	// for hash
	std::unordered_map<VertexPTN, int, VertexPTNHash, VertexPTNEquals> vertexMap;

	// hash 
	int GetVertexIndexHash(const VertexPTN& vertex);

	int GetNumVertices() const { return numVertices; }
	int GetNumTriangles() const { return numTriangles; }
	int GetNumSubMeshes() const { return (int)subMeshes.size(); }

	glm::vec3 GetObjCenter() const { return objCenter; }
	glm::vec3 GetObjExtent() const { return objExtent; }

private:

	//store tmp data
	std::vector<glm::vec3> v_data;
	std::vector<glm::vec3> vn_data;
	std::vector<glm::vec2> vt_data;

	//store vertex buffer
	GLuint vboId;

	//all vertices
	std::vector<VertexPTN> vertices;

	//meshes that use different materail
	std::vector<SubMesh> subMeshes;

	std::unordered_map<std::string, PhongMaterial*> mtl;
	//total num of vertices (vertives.size())
	int numVertices;

	//total num of triangles ( sum of num of triangles in each submesh)
	int numTriangles;

	glm::vec3 objCenter;
	glm::vec3 objExtent;
};


#endif
