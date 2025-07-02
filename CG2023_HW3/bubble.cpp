#include "Bubble.h"

float randomFloat(float min, float max) {

    float scale = rand() / (float)RAND_MAX;
    return min + scale * (max - min);
}


Bubble::Bubble(const std::string& texImagePath) {

    panorama = new ImageTexture(texImagePath);

    material = new BubbleMaterial();

    material->SetMapKd(panorama);

    CreateSquare(36,18,2.0f,vertices, indices);

   glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPT__) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &iboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

    instanceMatrices.resize(5000); 
    for (int i = 0; i < 5000; ++i) {
        glm::vec3 position = glm::vec3(
            randomFloat(-2000.0f, 2000.0f), 
            randomFloat(-2000.0f, 2000.0f), 
            randomFloat(-2000.0f, 2000.0f)  
        );

        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
  
        instanceMatrices[i] = model;
    }

    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, instanceMatrices.size() * sizeof(glm::mat4), &instanceMatrices[0], GL_STATIC_DRAW);
}

Bubble::~Bubble() {
    vertices.clear();
    glDeleteBuffers(1, &vboId);
    indices.clear();
    glDeleteBuffers(1, &iboId);
    glDeleteBuffers(1, &instanceVBO);

    if (panorama) {
        delete panorama;
        panorama = nullptr;
    }
    if (material) {
        delete material;
        material = nullptr;
    }
}


void Bubble::Update() {

    int timeSinceStart = glutGet(GLUT_ELAPSED_TIME); 
    float currentTime = timeSinceStart / 1000.0f;  


    for (int i = 0; i < 5000; ++i) {
        float angle = currentTime + (float)i / 10000.0f * 2.0f * 3.1415926; 

        float radius = 1.0f; 

        glm::vec3 circlePosition = glm::vec3(cos(angle) * radius, 0.0f, sin(angle) * radius);

        glm::vec3 combinedPosition = circlePosition + glm::vec3(instanceMatrices[i][3]);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), combinedPosition);

        instanceMatrices[i] = model;
    }

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

    glBufferSubData(GL_ARRAY_BUFFER, 0, instanceMatrices.size() * sizeof(glm::mat4), &instanceMatrices[0]);

}

void Bubble::Render(Camera* camera, BubbleShaderProg* shader) {

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPT__), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPT__), (const GLvoid*)12);

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    for (int i = 0; i < 4; ++i) {
        glEnableVertexAttribArray(2 + i);
        glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(2 + i, 1);
    }

    shader->Bind();
    glUniformMatrix4fv(shader->GetLocMVP(), 1, GL_FALSE, glm::value_ptr(camera->GetProjMatrix() * camera->GetViewMatrix()));

    if (material->GetMapKd() != nullptr) {
        material->GetMapKd()->Bind(GL_TEXTURE0);
        glUniform1i(shader->GetLocMapKd(), 0);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
    glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0, instanceMatrices.size());

    shader->UnBind();

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    for (int i = 0; i < 4; ++i) {
        glDisableVertexAttribArray(2 + i);
    }

    for (int i = 0; i < 4; ++i) {
        glVertexAttribDivisor(2 + i, 0);
    }

}


void Bubble::CreateSquare(const int nSlices, const int nStacks, const float radius, std::vector<VertexPT__>& vertices, std::vector<unsigned int>& indices) {

    const int numPhi = nSlices;
    const int numTheta = nStacks;
    const float phiStart = 0.0f;
    const float thetaStart = 0.5f * glm::pi<float>();
    const float phiOffset = 2.0f * glm::pi<float>() / (float)numPhi;
    const float thetaOffset = -glm::pi<float>() / (float)numTheta;
    for (int t = 0; t <= numTheta; ++t) {
        for (int p = 0; p <= numPhi; ++p) {
            float phi = phiStart + p * phiOffset;
            float theta = thetaStart + t * thetaOffset;
            // std::cout << glm::degrees<float>(phi) << " " << glm::degrees<float>(theta) << std::endl;
            glm::vec2 uv = glm::vec2((float)p / (float)numPhi, (float)t / (float)numTheta);
            // std::cout << uv.x << " " << uv.y << std::endl;
            float x = radius * std::cosf(theta) * std::cosf(phi);
            float y = radius * std::sinf(theta);
            float z = radius * std::cosf(theta) * std::sinf(phi);

            VertexPT__ vt = VertexPT__(glm::vec3(x, y, z), uv);
            vertices.push_back(vt);
        }
    }

    // Vertex order (4 x 2 division for example): 
    //  0   1   2   3   4.
    //  5   6   7   8   9.
    // 10  11  12  13  14.
    const int pVertices = nSlices + 1;
    const int tVertices = nStacks + 1;
    for (int t = 0; t < nStacks; ++t) {
        for (int p = 0; p < nSlices; ++p) {
            // Each grid will generate 2 triangles.
            // The upper-half one.
            indices.push_back(t * pVertices + p);
            indices.push_back(t * pVertices + p + 1);
            indices.push_back((t + 1) * pVertices + p);
            // The bottom-half one.
            indices.push_back(t * pVertices + p + 1);
            indices.push_back((t + 1) * pVertices + p + 1);
            indices.push_back((t + 1) * pVertices + p);
        }
    }
}


