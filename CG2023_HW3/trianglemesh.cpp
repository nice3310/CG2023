#include "trianglemesh.h"
#include "material.h"

// Constructor of a triangle mesh.
TriangleMesh::TriangleMesh()
{
	numVertices = 0;
	numTriangles = 0;
	objCenter = glm::vec3(0.f, 0.0f, 0.0f);
	vboId = 0;
}


TriangleMesh::~TriangleMesh()
{
	// Clear vertices vector.
	vertices.clear();

	// Delete the VBO.
	glDeleteBuffers(1, &vboId);

	// Iterate over and clear subMeshes.
	for (auto& subMesh : subMeshes) {
		// Delete the IBO for each subMesh.
		if (subMesh.iboId != 0) {
			glDeleteBuffers(1, &subMesh.iboId);
		}
	}
	subMeshes.clear();
}

// Load the geometry and material data from an OBJ file.
bool TriangleMesh::LoadFromFile(const std::string& filePath, const bool normalized)
{
	std::ifstream inFile(filePath);
	std::string line;
	std::string currentMtlName;
	SubMesh* currentSubMesh = nullptr;

	while (std::getline(inFile, line)) {
		std::stringstream ss(line);
		std::string identifier;

		ss >> identifier;

		if (identifier == "v") {
			float x, y, z;
			ss >> x >> y >> z;
			v_data.push_back(glm::vec3(x, y, z));
		}

		else if (identifier == "vt") {
			float x, y;
			ss >> x >> y;
			vt_data.push_back(glm::vec2(x, y));
		}
		else if (identifier == "vn") {
			float x, y, z;
			ss >> x >> y >> z;
			vn_data.push_back(glm::vec3(x, y, z));
		}

		// Process .mtl File
		else if (identifier == "mtllib") {
			std::string name;
			ss >> name;
			std::string Path = "mtl_file/" + name;
			parseMTL(Path);
		}

		// New and Process SubMesh
		else if (identifier == "usemtl") {

			if (currentSubMesh != nullptr) {

				subMeshes.push_back(*currentSubMesh);

				delete currentSubMesh;
			}

			ss >> currentMtlName;

			currentSubMesh = new SubMesh();

			currentSubMesh->material = mtl[currentMtlName];

		}
		else if (identifier == "f") {

			std::vector<int> PolyGonIndices;

			while (!ss.eof()) {
				int vIndex, vnIndex, vtIndex;
				ss >> vIndex;
				ss.ignore(1);
				ss >> vtIndex;
				ss.ignore(1);
				ss >> vnIndex;

				VertexPTN newVertex = VertexPTN(
					glm::vec3(v_data[vIndex - 1].x, v_data[vIndex - 1].y, v_data[vIndex - 1].z),
					glm::vec3(vn_data[vnIndex - 1].x, vn_data[vnIndex - 1].y, vn_data[vnIndex - 1].z),
					glm::vec2(vt_data[vtIndex - 1].x, vt_data[vtIndex - 1].y)
				);

				//Polygon Decomposition O(1)
				int index = GetVertexIndexHash(newVertex);

				if (index == -1) {
					vertices.push_back(newVertex);
					PolyGonIndices.push_back(numVertices);
					numVertices += 1;
				}
				else {
					PolyGonIndices.push_back(index);
				}
			}

			//Make A Triangle
			for (size_t i = 1; i < PolyGonIndices.size() - 1; ++i) {

				currentSubMesh->vertexIndices.push_back(PolyGonIndices[0]);
				currentSubMesh->vertexIndices.push_back(PolyGonIndices[i]);
				currentSubMesh->vertexIndices.push_back(PolyGonIndices[i + 1]);
				numTriangles += 1;
			}
		}
	}

	//the last one need to be push_back into subMeshes too 
	if (currentSubMesh != nullptr) {
		subMeshes.push_back(*currentSubMesh);
		delete currentSubMesh;
	}

	if (normalized) {
		//Find the bounds and center of the model

		glm::vec3 minBound = vertices[0].position;
		glm::vec3 maxBound = vertices[0].position;

		for (const auto& vertex : vertices) {
			minBound.x = std::min(minBound.x, vertex.position.x);
			minBound.y = std::min(minBound.y, vertex.position.y);
			minBound.z = std::min(minBound.z, vertex.position.z);

			maxBound.x = std::max(maxBound.x, vertex.position.x);
			maxBound.y = std::max(maxBound.y, vertex.position.y);
			maxBound.z = std::max(maxBound.z, vertex.position.z);
		}

		glm::vec3 center = (maxBound + minBound) * 0.5f;

		//Calculate the scale factor based on the longest axis
		glm::vec3 extents = maxBound - minBound;
		float maxExtent = std::max({ extents.x, extents.y, extents.z });
		float scaleFactor = 1.0f / maxExtent;

		//Normalize the vertices
		for (auto& vertex : vertices) {
			vertex.position = (vertex.position - center) * scaleFactor;
		}


		glm::vec3 newMinBound = vertices[0].position;
		glm::vec3 newMaxBound = vertices[0].position;

		for (const auto& vertex : vertices) {
			newMinBound.x = std::min(newMinBound.x, vertex.position.x);
			newMinBound.y = std::min(newMinBound.y, vertex.position.y);
			newMinBound.z = std::min(newMinBound.z, vertex.position.z);

			newMaxBound.x = std::max(newMaxBound.x, vertex.position.x);
			newMaxBound.y = std::max(newMaxBound.y, vertex.position.y);
			newMaxBound.z = std::max(newMaxBound.z, vertex.position.z);
		}

		objCenter = (newMaxBound + newMinBound) * 0.5f;
		objExtent = newMaxBound - newMinBound;
	}

	return true;
}

void TriangleMesh::parseMTL(const std::string& filename) {

	std::ifstream inFile(filename);
	std::string line;
	PhongMaterial* currentMaterial = nullptr;

	while (std::getline(inFile, line)) {
		std::stringstream ss(line);
		std::string identifier;
		float Ns;
		float x;
		float y;
		float z;
		ss >> identifier;

		if (identifier == "newmtl") {
			std::string name;
			ss >> name;
			currentMaterial = new PhongMaterial();
			currentMaterial->SetName(name);
			mtl[name] = currentMaterial;
		}

		else if (currentMaterial) {

			if (identifier == "Ns") {
				ss >> Ns;
				currentMaterial->SetNs(Ns);
			}
			else if (identifier == "Ka") {
				ss >> x >> y >> z;
				currentMaterial->SetKa(glm::vec3(x, y, z));
			}
			else if (identifier == "Kd") {
				ss >> x >> y >> z;
				currentMaterial->SetKd(glm::vec3(x, y, z));
			}
			else if (identifier == "Ks") {
				ss >> x >> y >> z;
				currentMaterial->SetKs(glm::vec3(x, y, z));
			}
			else if (identifier == "map_Kd") { 
				std::string map;
				ss >> map;
				ImageTexture* NewMap = new ImageTexture(map);
				currentMaterial->SetMapKd(NewMap);
			}
		}
	}
}

void TriangleMesh::ShowMTL() {

	for (auto it = mtl.begin(); it != mtl.end(); ++it) {
		std::cout << "\n";
		std::cout << "Material Name: " << it->first << "\n";
		std::cout << "Ns: " << it->second->GetNs() << "\n";
		std::cout << "Ka x: " << it->second->GetKa().x << " ";
		std::cout << "Ka y: " << it->second->GetKa().y << " ";
		std::cout << "Ka z: " << it->second->GetKa().z << "\n";
		std::cout << "Kd x: " << it->second->GetKd().x << " ";
		std::cout << "Kd y: " << it->second->GetKd().y << " ";
		std::cout << "Kd z: " << it->second->GetKd().z << "\n";
		std::cout << "Ks x: " << it->second->GetKs().x << " ";
		std::cout << "Ks y: " << it->second->GetKs().y << " ";
		std::cout << "Ks z: " << it->second->GetKs().z << "\n";
	}
	
}


void TriangleMesh::CreateBuffer() {
	// Create vertex buffer.
	glGenBuffers(1, &vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPTN) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// Create index buffer.
	for (auto& mesh : subMeshes) {
		glGenBuffers(1, &mesh.iboId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.iboId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh.vertexIndices.size(), &(mesh.vertexIndices[0]), GL_STATIC_DRAW);
	}

}

void TriangleMesh::ShowInfo()
{
	std::cout << "# Vertices: " << numVertices << std::endl;
	std::cout << "# Triangles: " << numTriangles << std::endl;
	std::cout << "Total " << subMeshes.size() << " subMeshes loaded" << std::endl;
	for (unsigned int i = 0; i < subMeshes.size(); ++i) {
		const SubMesh& g = subMeshes[i];
		std::cout << "Num. triangles in the subMesh: " << g.vertexIndices.size() / 3 << std::endl;
	}
	std::cout << "Model Center: " << objCenter.x << ", " << objCenter.y << ", " << objCenter.z << std::endl;
	std::cout << "Model Extent: " << objExtent.x << " x " << objExtent.y << " x " << objExtent.z << std::endl;
}

// find or create the index of vertex
int TriangleMesh::GetVertexIndexHash(const VertexPTN& vertex) {
	auto it = vertexMap.find(vertex);
	if (it != vertexMap.end()) {
		return it->second;
	}
	else {
		int newIndex = numVertices;
		vertexMap[vertex] = newIndex;
		return -1;
	}
}

//Get Ka, Kd, Ks, Ns from specific matetial here
void TriangleMesh::Render(PhongShadingDemoShaderProg* shader) {


	glEnableVertexAttribArray(0);

	glEnableVertexAttribArray(1);

	glEnableVertexAttribArray(2); 


	glBindBuffer(GL_ARRAY_BUFFER, vboId);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (void*)offsetof(VertexPTN, position));

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (void*)offsetof(VertexPTN, normal));

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (void*)offsetof(VertexPTN, texcoord)); 


	for (const auto& mesh : subMeshes) {

		if (mesh.material) {

			glUniform3fv(shader->GetLocKa(), 1, glm::value_ptr(mesh.material->GetKa()));
			glUniform3fv(shader->GetLocKd(), 1, glm::value_ptr(mesh.material->GetKd()));
			glUniform3fv(shader->GetLocKs(), 1, glm::value_ptr(mesh.material->GetKs()));
			glUniform1f(shader->GetLocNs(), mesh.material->GetNs());
			if (mesh.material->GetMapKd() != nullptr) { 
				mesh.material->GetMapKd()->Bind(GL_TEXTURE0);
				glUniform1i(shader->GetLocMapKd(), 0);		
			}
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.iboId);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.vertexIndices.size()), GL_UNSIGNED_INT, 0);

	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2); 

}