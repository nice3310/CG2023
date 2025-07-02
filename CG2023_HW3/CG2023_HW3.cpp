#include <cmath>
#include "headers.h"
#include "trianglemesh.h"
#include "camera.h"
#include "shaderprog.h"
#include "light.h"
#include "imagetexture.h"
#include "skybox.h"
#include "line.h"
#include "bubble.h"
#include "fish.h"



glm::mat4 R = glm::rotate(glm::mat4(1.0f), glm::radians(300.0f), glm::vec3(1, 0, 0));
glm::mat4 RR = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1));
glm::mat4 RRR = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0, 1, 0));

//Object fixed Rotation
glm::mat4 GrayFishRotation = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));
glm::mat4 YellowFishRotation = R * RR * RRR;


// Global variables.
int screenWidth = 600;
int screenHeight = 600;

// Lights.
DirectionalLight* dirLight = nullptr;
PointLight* pointLight = nullptr;
SpotLight* spotLight = nullptr;
glm::vec3 dirLightDirection = glm::vec3(0.0f, -1.0f, 0.0f);
glm::vec3 dirLightRadiance = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 pointLightPosition = glm::vec3(0.8f, 0.0f, 0.8f);
glm::vec3 pointLightIntensity = glm::vec3(0.5f, 0.1f, 0.1f);
glm::vec3 spotLightPosition = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 spotLightDirection = glm::vec3(0.0f, -1.0f, 0.0f);
glm::vec3 spotLightIntensity = glm::vec3(0.25f, 0.25f, 0.1f);
float spotLightCutoffStartInDegree = 30.0f;
float spotLightTotalWidthInDegree = 45.0f;
glm::vec3 ambientLight = glm::vec3(0.2f, 0.2f, 0.2f);
glm::vec3 sdd = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 sdr = glm::vec3(1.0f, 1.0f, 1.0f);

// Camera.
Camera* camera = nullptr;
glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, -5.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float fovy = 30.0f;
float zNear = 0.1f;
float zFar = 10000.0f;

// Shader.
FillColorShaderProg* fillColorShader = nullptr;
PhongShadingDemoShaderProg* phongShadingShader = nullptr;
SkyboxShaderProg* skyboxShader = nullptr;
LineShaderProg* lineShader = nullptr;
BubbleShaderProg* bubbleShader = nullptr;
PhongShadingDemoShaderProg* fishShader = nullptr;

// UI.
const float lightMoveSpeed = 0.2f;

// Triangle mesh.
TriangleMesh* mesh = nullptr;
Fish* GrayFish = nullptr;
Fish* YellowFish = nullptr;

// Skybox.
Skybox* skybox = nullptr;

// Line
Line* line = nullptr;
Line* line2 = nullptr;

// Bubble
Bubble* bubble = nullptr;

// Rotation Map
static float MapRotation = 0.0f;
const float MapRotSetp = 0.2f;
bool needRotationMap = false;

// Render Line Control
int LineChangeCounter = 0;
bool LineChange = false;
bool needRenderLine = false;

//Keyboard Control
bool keyStates[256];

void InitializeKeyStates() {
    for (int i = 0; i < 256; i++) {
        keyStates[i] = false;
    }
}

// SceneObject.
struct SceneObject
{
    SceneObject() {
        mesh = nullptr;
        worldMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        worldCenter = glm::vec3(0.0f, 0.0f, 0.0f);
        forward = glm::vec3(0.0f, 0.0f, -1.0f);
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        targetForward = glm::vec3(0.0f, 0.0f, 1.0f);

        normalMoveSpeed = 5.0f;
        acceleratedMoveSpeed = 7.0f;
        currentMoveSpeed = normalMoveSpeed;

        normalTimeScale = 3.0f;
        acceleratedTimeScale = 3.5f;
        currentTimeScale = normalTimeScale;

        sidetoside = 0.1f;
        pivot = 0.1f;
        wave = 0.2f;
        twist = 0.3f;

        distanceBack = 5.0f;
        distanceUp = 1.0f;
    }

    // Perform spherical interpolation based on the direction to be adjusted, to serve as the animation for movement.

    void UpdateWorldMatrix_W() {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), worldCenter);

        glm::quat currentQuat = glm::quatLookAt(forward, up);
        glm::quat targetQuat = glm::quatLookAt(glm::normalize(targetForward), up);

        glm::quat interpolatedQuat = glm::slerp(currentQuat, targetQuat, 0.03f);

        forward = glm::mat3_cast(interpolatedQuat) * glm::vec3(0.0f, 0.0f, -1.0f);

        glm::vec3 worldForward = glm::normalize(forward);
        glm::vec3 worldUp = glm::normalize(up);
        glm::vec3 worldRight = glm::normalize(glm::cross(worldUp, worldForward));
        worldUp = glm::normalize(glm::cross(worldForward, worldRight));

        glm::mat4 rotation = glm::mat4(
            glm::vec4(worldRight, 0.0f),
            glm::vec4(worldUp, 0.0f),
            glm::vec4(worldForward, 0.0f),
            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
        );

        worldMatrix = translation * rotation;
    }
    void UpdateWorldMatrix_S() {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), worldCenter);

        glm::quat currentQuat = glm::quatLookAt(forward, up);
        glm::quat targetQuat = glm::quatLookAt(glm::normalize(-targetForward), up);

        glm::quat interpolatedQuat = glm::slerp(currentQuat, targetQuat, 0.03f);

        forward = glm::mat3_cast(interpolatedQuat) * glm::vec3(0.0f, 0.0f, -1.0f);

        glm::vec3 worldForward = glm::normalize(forward);
        glm::vec3 worldUp = glm::normalize(up);
        glm::vec3 worldRight = glm::normalize(glm::cross(worldUp, worldForward));
        worldUp = glm::normalize(glm::cross(worldForward, worldRight));

        glm::mat4 rotation = glm::mat4(
            glm::vec4(worldRight, 0.0f),
            glm::vec4(worldUp, 0.0f),
            glm::vec4(worldForward, 0.0f),
            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
        );

        worldMatrix = translation * rotation;
    }
    void UpdateWorldMatrix_D() {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), worldCenter);
        glm::vec3 targetDirection = glm::normalize(glm::cross(targetForward, up));
        glm::quat currentQuat = glm::quatLookAt(forward, up);
        glm::quat targetQuat = glm::quatLookAt(targetDirection, up);
        glm::quat interpolatedQuat = glm::slerp(currentQuat, targetQuat, 0.03f);
        forward = glm::mat3_cast(interpolatedQuat) * glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 worldForward = glm::normalize(forward);
        glm::vec3 worldUp = glm::normalize(up);
        glm::vec3 worldRight = glm::normalize(glm::cross(worldUp, worldForward));
        worldUp = glm::normalize(glm::cross(worldForward, worldRight));

        glm::mat4 rotation = glm::mat4(
            glm::vec4(worldRight, 0.0f),
            glm::vec4(worldUp, 0.0f),
            glm::vec4(worldForward, 0.0f),
            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
        );

        worldMatrix = translation * rotation;
    }
    void UpdateWorldMatrix_A() {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), worldCenter);

        glm::vec3 targetDirection = glm::normalize(glm::cross(up, targetForward));
        glm::quat currentQuat = glm::quatLookAt(forward, up);
        glm::quat targetQuat = glm::quatLookAt(targetDirection, up);
        glm::quat interpolatedQuat = glm::slerp(currentQuat, targetQuat, 0.03f);
        forward = glm::mat3_cast(interpolatedQuat) * glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 worldForward = glm::normalize(forward);
        glm::vec3 worldUp = glm::normalize(up);
        glm::vec3 worldRight = glm::normalize(glm::cross(worldUp, worldForward));
        worldUp = glm::normalize(glm::cross(worldForward, worldRight));

        glm::mat4 rotation = glm::mat4(
            glm::vec4(worldRight, 0.0f),
            glm::vec4(worldUp, 0.0f),
            glm::vec4(worldForward, 0.0f),
            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
        );

        worldMatrix = translation * rotation;
    }

    // Fish move
    void Move() {

        glm::vec3 movementDirection = glm::vec3(0.0f);

        // movementDirection accumulation
        if (keyStates['w'] || keyStates['W']) {
            movementDirection += forward;
        }
        if (keyStates['s'] || keyStates['S']) {
            movementDirection += forward;
        }
        if (keyStates['a'] || keyStates['A']) {
            movementDirection += glm::normalize(glm::cross(up, targetForward));
        }
        if (keyStates['d'] || keyStates['D']) {
            movementDirection += glm::normalize(glm::cross(targetForward,up));
        }

        //To perform normalization, otherwise, pressing two directional vectors at the same time will cause their lengths to be superimposed, 
        // resulting in increased speed.
        if (glm::length(movementDirection) > 0) {
            movementDirection = glm::normalize(movementDirection) * currentMoveSpeed;
            worldCenter += movementDirection;
            if (keyStates['w'] || keyStates['W']) UpdateWorldMatrix_W();
            if (keyStates['s'] || keyStates['S']) UpdateWorldMatrix_S();
            if (keyStates['a'] || keyStates['A']) UpdateWorldMatrix_A();
            if (keyStates['d'] || keyStates['D']) UpdateWorldMatrix_D();
            //line->SetWorldMatrix(worldMatrix);
            UpdateCamera();
        }
    }

    //interpolate speed and time scale, also enable rendering speed line
    void UpdateSpeedAndTimeScale() {

        float speedLerpFactor = 0.005f;

        if (keyStates['q'] || keyStates['Q']) {
            needRenderLine = true;
            currentMoveSpeed = glm::mix(currentMoveSpeed, acceleratedMoveSpeed, speedLerpFactor);
            currentTimeScale = glm::mix(currentTimeScale, acceleratedTimeScale, speedLerpFactor);
        }
        else {
            currentMoveSpeed = glm::mix(currentMoveSpeed, normalMoveSpeed, speedLerpFactor);
            currentTimeScale = glm::mix(currentTimeScale, normalTimeScale, speedLerpFactor);
        }
    }

    //third person camera based on sceneObj
    void UpdateCamera() {
        glm::vec3 cameraPosition = worldCenter - targetForward * distanceBack;
        cameraPosition += glm::vec3(0, 1, 0) * distanceUp;
        glm::vec3 cameraTarget = worldCenter;
        glm::vec3 upVector = glm::vec3(0, 1, 0);
        camera->UpdateView(cameraPosition, cameraTarget, upVector);
    }

    TriangleMesh* mesh;

    //position and direction
    glm::vec3 targetForward;
    glm::mat4x4 worldMatrix;
    glm::vec3 worldCenter;
    glm::vec3 forward;
    glm::vec3 up;

    //peed used to interpolate
    float normalMoveSpeed;
    float acceleratedMoveSpeed;
    float currentMoveSpeed;

    //time scale used to interpolate
    float normalTimeScale;
    float acceleratedTimeScale;
    float currentTimeScale;

    // animation parameters
    float sidetoside;
    float pivot;
    float wave;
    float twist;
    float black;
    float white;

    //third person camera setting 
    float distanceBack;
    float distanceUp;
};

SceneObject sceneObj;

// ScenePointLight (for visualization of a point light).
struct ScenePointLight
{
    ScenePointLight() {
        light = nullptr;
        worldMatrix = glm::mat4x4(1.0f);
        visColor = glm::vec3(1.0f, 1.0f, 1.0f);
    }
    PointLight* light;
    glm::mat4x4 worldMatrix;
    glm::vec3 visColor;
};
ScenePointLight pointLightObj;
ScenePointLight spotLightObj;

// Function prototypes.
void ReleaseResources();
// Callback functions.
void RenderSceneCB();
void ReshapeCB(int, int);
void ProcessSpecialKeysCB(int, int, int);
void ProcessKeysCB(unsigned char, int, int);
void SetupRenderState();
void LoadObjects(const std::string&);
void CreateCamera();
void CreateSkybox(const std::string);
void CreateShaderLib();
void ReleaseResources()
{
    // Delete scene objects and lights.
    if (mesh != nullptr) {
        delete mesh;
        mesh = nullptr;
    }
    if (pointLight != nullptr) {
        delete pointLight;
        pointLight = nullptr;
    }
    if (dirLight != nullptr) {
        delete dirLight;
        dirLight = nullptr;
    }
    if (spotLight != nullptr) {
        delete spotLight;
        spotLight = nullptr;
    }
    // Delete camera.
    if (camera != nullptr) {
        delete camera;
        camera = nullptr;
    }
    // Delete shaders.
    if (fillColorShader != nullptr) {
        delete fillColorShader;
        fillColorShader = nullptr;
    }
    if (phongShadingShader != nullptr) {
        delete phongShadingShader;
        phongShadingShader = nullptr;
    }
    if (skyboxShader != nullptr) {
        delete skyboxShader;
        skyboxShader = nullptr;
    }
    if (lineShader != nullptr) {
        delete lineShader;
        lineShader = nullptr;
    }
    if (bubbleShader != nullptr) {
        delete bubbleShader;
        bubbleShader = nullptr;
    }
    if (fishShader != nullptr) {
        delete fishShader;
        fishShader = nullptr;
    }
}

void RenderSceneCB()

{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    LineChangeCounter += 1;

    float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    if (needRotationMap) MapRotation += MapRotSetp;

    skybox->SetRotation(MapRotation);

    phongShadingShader->Bind();

    glm::mat4x4 normalMatrix = glm::transpose(glm::inverse(sceneObj.worldMatrix));
    glm::mat4x4 MVP = camera->GetProjMatrix() * camera->GetViewMatrix() * sceneObj.worldMatrix;
    sceneObj.UpdateSpeedAndTimeScale();
    sceneObj.Move();
    glUniform1f(phongShadingShader->GetLocTime(), time);
    glUniform1f(phongShadingShader->GetLocTimeScale(), sceneObj.currentTimeScale);
    glUniform1f(phongShadingShader->GetLocSideToSide(), sceneObj.sidetoside);
    glUniform1f(phongShadingShader->GetLocPivot(), sceneObj.pivot);
    glUniform1f(phongShadingShader->GetLocWave(), sceneObj.wave);
    glUniform1f(phongShadingShader->GetLocTwist(), sceneObj.twist);
    glUniformMatrix4fv(phongShadingShader->GetLocM(), 1, GL_FALSE, glm::value_ptr(sceneObj.worldMatrix));
    glUniform3fv(phongShadingShader->GetLocCameraPos(), 1, glm::value_ptr(camera->GetCameraPos()));
    glUniformMatrix4fv(phongShadingShader->GetLocNM(), 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glUniformMatrix4fv(phongShadingShader->GetLocMVP(), 1, GL_FALSE, glm::value_ptr(MVP));

    if (dirLight != nullptr) {     
        dirLight->SetDirection(glm::normalize(sceneObj.targetForward));
        glUniform3fv(phongShadingShader->GetLocDirLightDir(), 1, glm::value_ptr(dirLight->GetDirection()));
        glUniform3fv(phongShadingShader->GetLocDirLightRadiance(), 1, glm::value_ptr(dirLight->GetRadiance()));
    }

    if (pointLight != nullptr) {
        glUniform3fv(phongShadingShader->GetLocPointLightPos(), 1, glm::value_ptr(pointLight->GetPosition()));
        glUniform3fv(phongShadingShader->GetLocPointLightIntensity(), 1, glm::value_ptr(pointLight->GetIntensity()));
    }

    if (spotLight != nullptr) {
        glUniform3fv(phongShadingShader->GetLocSpotLightPosition(), 1, glm::value_ptr(spotLight->GetspotLightPosition()));
        glUniform3fv(phongShadingShader->GetLocSpotLightDirection(), 1, glm::value_ptr(spotLight->GetspotLightDirection()));
        glUniform3fv(phongShadingShader->GetLocSpotLightIntensity(), 1, glm::value_ptr(spotLight->GetspotLightIntensity()));
        glUniform1f(phongShadingShader->GetLocSpotLightCutoffStartInDegree(), spotLight->GetspotLightCutoffStartInDegree());
        glUniform1f(phongShadingShader->GetLocSpotLightTotalWidthInDegree(), spotLight->GetspotLightTotalWidthInDegree());
    }

    glUniform3fv(phongShadingShader->GetLocAmbientLight(), 1, glm::value_ptr(ambientLight));

    sceneObj.mesh->Render(phongShadingShader);

    phongShadingShader->UnBind();

    fishShader->Bind();

    glUniform3fv(fishShader ->GetLocCameraPos(), 1, glm::value_ptr(camera->GetCameraPos()));

    if (dirLight != nullptr) {
        glUniform3fv(fishShader->GetLocDirLightDir(), 1, glm::value_ptr(dirLight->GetDirection()));
        glUniform3fv(fishShader->GetLocDirLightRadiance(), 1, glm::value_ptr(dirLight->GetRadiance()));
    }

    glUniform3fv(fishShader->GetLocAmbientLight(), 1, glm::value_ptr(ambientLight));

    GrayFish->Update();

    YellowFish->Update();

    GrayFish->Render(camera, fishShader);

    YellowFish->Render(camera, fishShader);

    fishShader->UnBind();

    // Render skybox. ----------------------------------------------------------------------------
    if (skybox != nullptr) {
        // -------------------------------------------------------
        // Add your code to rotate the skybox.
        // -------------------------------------------------------
        skybox->Render(camera, skyboxShader);
    }
    // Render Bubble-------------------------------------------------------------------------------------------

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    bubble->Update();
    bubble->Render(camera, bubbleShader);

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);


    // Render Line. ----------------------------------------------------------------------------
    if (line != nullptr && line2!=nullptr && needRenderLine) {
        glDepthMask(GL_FALSE); 
  
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        if (LineChange) {
            line->Render(camera, lineShader);
        }
        else{
            line2->Render(camera, lineShader);
        }

        //use counter, otherwise it will update too frequently.
        if (LineChangeCounter >= 5) {
            LineChange = !LineChange;
            LineChangeCounter = 0;
        }
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
        needRenderLine = false;
    }
    // ------------------------------------------------------------------------------------------

    glutSwapBuffers();
}

void ReshapeCB(int w, int h)
{
    // Update viewport.
    screenWidth = w;
    screenHeight = h;
    glViewport(0, 0, screenWidth, screenHeight);
    // Adjust camera and projection.
    float aspectRatio = (float)screenWidth / (float)screenHeight;
    camera->UpdateProjection(fovy, aspectRatio, zNear, zFar);
}

void ProcessSpecialKeysCB(int key, int x, int y)
{
    // Handle special (functional) keyboard inputs such as F1, spacebar, page up, etc. 
    switch (key) {
        // Rendering mode.
    case GLUT_KEY_F1:
        // Render with point mode.
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        break;
    case GLUT_KEY_F2:
        // Render with line mode.
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case GLUT_KEY_F3:
        // Render with fill mode.
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;

    case GLUT_KEY_F4:
        needRotationMap = !needRotationMap;
        break;
    case GLUT_KEY_F5:
        if (skybox != nullptr) {
            delete skybox;
            CreateSkybox("textures/UnderSea.jpg");
        }
        break;
    case GLUT_KEY_F6:
        if (skybox != nullptr) {
            delete skybox;
            CreateSkybox("textures/photostudio_02_2k.png");
        }
        break;
    case GLUT_KEY_F7:
        if (skybox != nullptr) {
            delete skybox;
            CreateSkybox("textures/sunflowers_2k.png");
        }
        break;
    case GLUT_KEY_F8:
        if (skybox != nullptr) {
            delete skybox;
            CreateSkybox("textures/veranda_2k.png");
        }
        break;
        // Light control.
    case GLUT_KEY_LEFT:
        if (pointLight != nullptr)
            pointLight->MoveLeft(lightMoveSpeed);
        break;
    case GLUT_KEY_RIGHT:
        if (pointLight != nullptr)
            pointLight->MoveRight(lightMoveSpeed);
        break;
    case GLUT_KEY_UP:
        if (pointLight != nullptr)
            pointLight->MoveUp(lightMoveSpeed);
        break;
    case GLUT_KEY_DOWN:
        if (pointLight != nullptr)
            pointLight->MoveDown(lightMoveSpeed);
        break;
    default:
        break;
    }
}

//偵測按鍵
void ProcessKeysCB(unsigned char key, int x, int y) {
    keyStates[key] = true; 
    glutPostRedisplay();
}

//釋放按鍵
void ProcessKeysUpCB(unsigned char key, int x, int y) {
    keyStates[key] = false; 
}

void SetupRenderState()
{
    glEnable(GL_DEPTH_TEST);

    glm::vec4 clearColor = glm::vec4(0.44f, 0.57f, 0.75f, 1.00f);
    glClearColor(
        (GLclampf)(clearColor.r),
        (GLclampf)(clearColor.g),
        (GLclampf)(clearColor.b),
        (GLclampf)(clearColor.a)
    );
}

void LoadObjects(const std::string& modelPath)
{
    // -------------------------------------------------------
    // Note: you can change the code below if you want to load
    //       the model dynamically.
    // -------------------------------------------------------

    mesh = new TriangleMesh();
    mesh->LoadFromFile(modelPath, true);
    mesh->CreateBuffer();
    sceneObj.mesh = mesh;
}

void CreateLights()
{
    // Create a directional light.
    dirLight = new DirectionalLight(dirLightDirection, dirLightRadiance);
    // Create a point light.
    pointLight = new PointLight(pointLightPosition, pointLightIntensity);
    pointLightObj.light = pointLight;
    pointLightObj.visColor = glm::normalize((pointLightObj.light)->GetIntensity());
    // Create a spot light.
    spotLight = new SpotLight(spotLightPosition, spotLightIntensity, spotLightDirection,
        spotLightCutoffStartInDegree, spotLightTotalWidthInDegree);
    spotLightObj.light = spotLight;
    spotLightObj.visColor = glm::normalize((spotLightObj.light)->GetIntensity());
}

void CreateCamera()
{
    // Create a camera and update view and proj matrices.
    camera = new Camera((float)screenWidth / (float)screenHeight);
    camera->UpdateView(cameraPos, cameraTarget, cameraUp);
    float aspectRatio = (float)screenWidth / (float)screenHeight;
    camera->UpdateProjection(fovy, aspectRatio, zNear, zFar);
}

void CreateSkybox(const std::string texFilePath)
{
    // -------------------------------------------------------
    // Note: you can change the code below if you want to change
    //       the skybox texture dynamically.
    // -------------------------------------------------------

    const int numSlices = 36;
    const int numStacks = 18;
    const float radius = 3000.0f;
    skybox = new Skybox(texFilePath, numSlices, numStacks, radius);
}

void CreateLine(const std::string texFilePath, const std::string texFilePath2)
{
    line = new Line(texFilePath);
    line2 = new Line(texFilePath2);
}

void CreateBubble(const std::string texFilePath) {
    bubble = new Bubble(texFilePath);
}

void CreateShaderLib()
{
    fillColorShader = new FillColorShaderProg();
    if (!fillColorShader->LoadFromFiles("shaders/fixed_color.vs", "shaders/fixed_color.fs"))
        exit(1);

    phongShadingShader = new PhongShadingDemoShaderProg();
    if (!phongShadingShader->LoadFromFiles("shaders/phong_shading_demo.vs", "shaders/phong_shading_demo.fs"))
        exit(1);

    skyboxShader = new SkyboxShaderProg();
    if (!skyboxShader->LoadFromFiles("shaders/skybox.vs", "shaders/skybox.fs"))
        exit(1);

    lineShader = new LineShaderProg();
    if (!lineShader->LoadFromFiles("shaders/line.vs", "shaders/line.fs"))
        exit(1);

    bubbleShader = new BubbleShaderProg();
    if (!bubbleShader->LoadFromFiles("shaders/bubble.vs", "shaders/bubble.fs"))
        exit(1);

    fishShader = new PhongShadingDemoShaderProg();
    if (!fishShader ->LoadFromFiles("shaders/fish_shader.vs", "shaders/fish_shader.fs"))
        exit(1);
}

int lastX = -1, lastY = -1;

// zoom in zoom out
void MouseWheelFunc(int wheel, int direction, int x, int y) {
    if (direction < 0.0f) {
        sceneObj.distanceBack += 0.8f;
    }
    else {
        if (sceneObj.distanceBack >= 1.0f) {

            sceneObj.distanceBack -= 0.8f;
        }
    }
    sceneObj.UpdateCamera();
    glutPostRedisplay();
}

// capture mouse position and set targetForward
void PassiveMotionCB(int x, int y) {
    static const float rotationSpeed = 1.0f;
    static const float maxVerticalAngle = 89.0f; // 垂直的最大角度
    static float currentVerticalAngle = 0.0f;    // 目前的垂直旋轉角度

    if (lastX >= 0 && lastY >= 0) {
        int deltaX = x - lastX;
        int deltaY = y - lastY;

        float horizontalAngle = -deltaX * rotationSpeed;

        float verticalAngle = deltaY * rotationSpeed;
        currentVerticalAngle += verticalAngle;
        if (currentVerticalAngle > maxVerticalAngle) {
            verticalAngle -= currentVerticalAngle - maxVerticalAngle;
            currentVerticalAngle = maxVerticalAngle;
        }
        else if (currentVerticalAngle < -maxVerticalAngle) {
            verticalAngle -= currentVerticalAngle + maxVerticalAngle;
            currentVerticalAngle = -maxVerticalAngle;
        }

        glm::vec3 right = glm::normalize(glm::cross(sceneObj.targetForward, sceneObj.up));
        glm::mat4 horizontalRot = glm::rotate(glm::mat4(1.0f), glm::radians(horizontalAngle), sceneObj.up);
        glm::mat4 verticalRot = glm::rotate(glm::mat4(1.0f), glm::radians(-verticalAngle), right);

        sceneObj.targetForward = glm::mat3(horizontalRot * verticalRot) * sceneObj.targetForward;

        //sceneObj.UpdateWorldMatrix(); //只能看到魚屁股

        sceneObj.UpdateCamera();
    }
    lastX = x;
    lastY = y;
}


int main(int argc, char** argv)
{

    srand(static_cast<unsigned int>(time(0)));
    // Setting window properties.
    glutInit(&argc, argv);
    glutSetOption(GLUT_MULTISAMPLE, 4);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Texture Mapping");
    glutSetCursor(GLUT_CURSOR_NONE);


    // Initialize GLEW.
    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        std::cerr << "GLEW initialization error: "
            << glewGetErrorString(res) << std::endl;
        return 1;
    }

    // Initialization.
    SetupRenderState();

    //object
    LoadObjects("obj_file/MagikarpF.obj");

    //bubble
    CreateBubble("textures/bubble.png");

    //skybox
    CreateSkybox("textures/UnderSea.jpg");

    //speed line
    CreateLine("textures/line.png","textures/line2.png");

    //fish
    GrayFish = new Fish(250,20, GrayFishRotation);
    GrayFish->LoadFromFile("obj_file/13007_Blue-Green_Reef_Chromis_v2_l3.obj", true);
    GrayFish->CreateBuffer();

    YellowFish = new Fish(300, 20, YellowFishRotation);
    YellowFish->LoadFromFile("obj_file/12265_Fish_v1_L2.obj", true);
    YellowFish->CreateBuffer();

    CreateLights();
    CreateCamera();
    CreateShaderLib();



    // Register callback functions.
    glutDisplayFunc(RenderSceneCB);
    glutIdleFunc(RenderSceneCB);
    glutReshapeFunc(ReshapeCB);
    glutSpecialFunc(ProcessSpecialKeysCB);
    glutKeyboardFunc(ProcessKeysCB);
    glutMouseWheelFunc(MouseWheelFunc);
    InitializeKeyStates(); 
    glutKeyboardUpFunc(ProcessKeysUpCB); 
    glutPassiveMotionFunc(PassiveMotionCB);


    // Start rendering loop.
    glutMainLoop();

    return 0;
}
