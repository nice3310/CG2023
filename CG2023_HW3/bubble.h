#ifndef BUBBLE_H
#define BUBBLE_H

#include "headers.h"
#include "imagetexture.h"
#include "shaderprog.h"
#include "material.h"
#include "camera.h"

// VertexPT Declarations.
struct VertexPT__ {
    glm::vec3 position;
    glm::vec2 texcoord;

    VertexPT__() : position(0.0f, 0.0f, 0.0f), texcoord(0.0f, 0.0f) {}
    VertexPT__(glm::vec3 p, glm::vec2 uv) : position(p), texcoord(uv) {}
};

class Bubble {
public:
    Bubble(const std::string& texImagePath);
    ~Bubble();
    void Render(Camera* camera, BubbleShaderProg* shader);
    void Update();


private:
    static void CreateSquare(const int nSlices, const int nStacks, const float radius, std::vector<VertexPT__>& vertices, std::vector<unsigned int>& indices);

    GLuint vboId, iboId, instanceVBO;
    std::vector<VertexPT__> vertices;
    std::vector<unsigned int> indices;
    std::vector<glm::mat4> instanceMatrices;

    BubbleMaterial* material;
    ImageTexture* panorama;
};

#endif
