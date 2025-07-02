#include "Line.h"

Line::Line(const std::string& texImagePath)
{
	rotationY = 0.0f;

	panorama = new ImageTexture(texImagePath);
	//panorama->Preview();

	// Create material.
	material = new LineMaterial();
	material->SetMapKd(panorama);

	// Create sphere geometry.
	CreateSquare(vertices, indices);

	// Create vertex buffer.
	glGenBuffers(1, &vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPT_) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	// Create index buffer.
	glGenBuffers(1, &iboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &(indices[0]), GL_STATIC_DRAW);
}

Line::~Line()
{
	vertices.clear();
	glDeleteBuffers(1, &vboId);
	indices.clear();
	glDeleteBuffers(1, &iboId);

	if (panorama) {
		delete panorama;
		panorama = nullptr;
	}
	if (material) {
		delete material;
		material = nullptr;
	}
}

void Line::Render(Camera* camera, LineShaderProg* shader)
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPT_), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPT_), (const GLvoid*)12);

	shader->Bind();
	
	// //for following the model, but it's quiet wired
	//glm::vec3 scaleVector = glm::vec3(2.5f, 2.5f, 2.5f);
	//glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scaleVector);
	//glm::mat4x4 MVP = camera->GetProjMatrix() * camera->GetViewMatrix() * WorldMatrix * scaleMatrix;

	// Render the whole window
	glm::mat4x4 MVP = glm::mat4(1.0f) ;

	// -------------------------------------------------------
	glUniformMatrix4fv(shader->GetLocMVP(), 1, GL_FALSE, glm::value_ptr(MVP));
	// Set material properties.
	if (material->GetMapKd() != nullptr) {
		material->GetMapKd()->Bind(GL_TEXTURE0);
		glUniform1i(shader->GetLocMapKd(), 0);
	}

	// Draw.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
	glDrawElements(GL_TRIANGLES, (GLsizei)(indices.size()), GL_UNSIGNED_INT, 0);

	shader->UnBind();

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}


void Line::CreateSquare(std::vector<VertexPT_>& vertices, std::vector<unsigned int>& indices)
{

	// //傳入SceneObj的 worldMatrix，更新MVP矩陣，並配合ScaleMatrix，讓速度線可以跟著模型走，但有點醜
	//	VertexPT_ v1 = VertexPT_(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec2(0.0f, 1.0f));
	//	VertexPT_ v2 = VertexPT_(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0.0f, 0.0f));
	//	VertexPT_ v3 = VertexPT_(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 0.0f));
	//	VertexPT_ v4 = VertexPT_(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec2(1.0f, 1.0f));

	//直接在NDC上Render，MVP矩陣設為 mat4x4(1.0f)
	VertexPT_ v1 = VertexPT_(glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)); // Top-Left
	VertexPT_ v2 = VertexPT_(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)); // Bottom-Left
	VertexPT_ v3 = VertexPT_(glm::vec3(1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)); // Bottom-Right
	VertexPT_ v4 = VertexPT_(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)); // Top-Right

	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v4);

	// Triangle 1
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(3);

	// Triangle 2
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);
}