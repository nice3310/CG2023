#include "trianglemesh.h"
#include "Fish.h"


float randomFloat_(float min, float max) {

	float scale = rand() / (float)RAND_MAX;
	return min + scale * (max - min);
}


Fish::Fish(float ra ,float sc, glm::mat4 roo)
{
	numVertices = 0;
	numTriangles = 0;
	objCenter = glm::vec3(0.f, 0.0f, 0.0f);
	vboId = 0;
	radius = ra;
	ObjRotationMatrix = roo;
	scale = sc;



}

Fish::~Fish()
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

bool Fish::LoadFromFile(const std::string& filePath, const bool normalized)
{
	std::ifstream inFile(filePath);
	std::string line;
	std::string currentMtlName;
	SubMesh_* currentSubMesh = nullptr;

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

			currentSubMesh = new SubMesh_();

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

				VertexPTN_ newVertex = VertexPTN_(
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

void Fish::parseMTL(const std::string& filename) {

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

void Fish::CreateBuffer() {
	// Create vertex buffer.
	glGenBuffers(1, &vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPTN_) * vertices.size(), &vertices[0], GL_STATIC_DRAW);


	for (auto& mesh : subMeshes) {
		glGenBuffers(1, &mesh.iboId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.iboId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh.vertexIndices.size(), &(mesh.vertexIndices[0]), GL_STATIC_DRAW);
	}


	instanceMatrices.resize(2000);
	for (int i = 0; i < 2000; ++i) {
		glm::vec3 position = glm::vec3(
			randomFloat_(-100.0f, 100.0f),
			randomFloat_(-100.0f, 100.0f),
			randomFloat_(-100.0f, 100.0f)
		);

		glm::mat4 model = glm::translate(glm::mat4(1.0f), position);

		instanceMatrices[i] = model;


	}

	// Create index buffer.
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, instanceMatrices.size() * sizeof(glm::mat4), &instanceMatrices[0], GL_STATIC_DRAW);
}

void Fish::Update() {
	int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);

	float currentTime = timeSinceStart / 1000.0f; 

	float speedFactor = 0.1f;

	for (int i = 0; i < 2000; ++i) {

		float angle = speedFactor * currentTime + ((float)i / 2000.0f) * 2.0f * 3.1415926;

		glm::vec3 horizontalPosition = glm::vec3(cos(angle), 0.0f, sin(angle)) * radius;

		//horizontalPosition.y = instanceMatrices[i][3][1];

		glm::vec3 combinedPosition = horizontalPosition;

		glm::vec3 tangent = glm::normalize(glm::vec3(-sin(angle), 0.0f, cos(angle)));

		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

		glm::mat4 viewMatrix = glm::lookAt(combinedPosition, combinedPosition + tangent, up);

		glm::mat4 rotationMatrix = glm::inverse(viewMatrix);

		glm::mat4 flipRotation = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		rotationMatrix = rotationMatrix * flipRotation;

		combinedPosition.y = instanceMatrices[i][3][1];

		glm::mat4 model = glm::translate(glm::mat4(1.0f), combinedPosition) * rotationMatrix * ObjRotationMatrix;

		model = glm::scale(model, glm::vec3(scale, scale, scale));

		instanceMatrices[i] = model;
	}

	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, instanceMatrices.size() * sizeof(glm::mat4), &instanceMatrices[0], GL_STATIC_DRAW);
}


int Fish::GetVertexIndexHash(const VertexPTN_& vertex) {
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

void Fish::Render(Camera* camera, PhongShadingDemoShaderProg* shader)
{

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN_), (void*)offsetof(VertexPTN_, position));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN_), (void*)offsetof(VertexPTN_, normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPTN_), (void*)offsetof(VertexPTN_, texcoord));

	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	for (int i = 0; i < 4; ++i) {
		glEnableVertexAttribArray(3 + i);
		glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
		glVertexAttribDivisor(3 + i, 1);
	}

	shader->Bind();

	glUniformMatrix4fv(shader->GetLocMVP(), 1, GL_FALSE, glm::value_ptr(camera->GetProjMatrix() * camera->GetViewMatrix()));

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

		glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)mesh.vertexIndices.size(), GL_UNSIGNED_INT, 0, instanceMatrices.size());
		//(GL_TRIANGLES, (GLsizei)mesh.vertexIndices.size(), GL_UNSIGNED_INT, 0);
	}
	shader->UnBind();

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	for (int i = 0; i < 4; ++i) {
		glDisableVertexAttribArray(3 + i);
	}

	for (int i = 0; i < 4; ++i) {
		glVertexAttribDivisor(3 + i, 0);
	}
}