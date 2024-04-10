#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;
const unsigned int NUM_LIGHT_CUBES = 2;

// camera
Camera camera(glm::vec3(0.0f, 1.0f, 6.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


int main() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "cg-course-project", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Leaving this comment here intentionally.
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model)
    // no - models in this project have textures with different texture origin
//    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    Shader platform1Shader("resources/shaders/platform1.vs", "resources/shaders/platform1.fs");
    Shader platform2Shader("resources/shaders/platform2.vs", "resources/shaders/platform2.fs");
    Shader wall1Shader("resources/shaders/wall1.vs", "resources/shaders/wall1.fs" );
    Shader wall2Shader("resources/shaders/wall2.vs", "resources/shaders/wall2.fs" );
    Shader stairsShader("resources/shaders/stairs.vs", "resources/shaders/stairs.fs");
    Shader lightCubeShader("resources/shaders/lightCube.vs", "resources/shaders/lightCube.fs");
    Shader modelShader("resources/shaders/model.vs", "resources/shaders/model.fs");

    // basic cube vertices - to be used for drawing platforms
    float platformVertices[] = {
            // positions                // normals              //texture coords
            // back face (CCW winding)
            0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  0.0f, 0.0f, // bottom-left
            -0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 4.0f, 0.0f, // bottom-right
            -0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 4.0f, 4.0f, // top-right
            -0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 4.0f, 4.0f, // top-right
            0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 4.0f, // top-left
            0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            // front face (CCW winding)
            -0.5f, -0.5f,  0.5f, 0.0f,  0.0f,  1.0f,  0.0f, 0.0f, // bottom-left
            0.5f, -0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 4.0f, 0.0f, // bottom-right
            0.5f,  0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 4.0f, 4.0f, // top-right
            0.5f,  0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 4.0f, 4.0f, // top-right
            -0.5f,  0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 0.0f, 4.0f, // top-left
            -0.5f, -0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face (CCW)
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 4.0f, 0.0f, // bottom-right
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 4.0f, 4.0f, // top-right
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 4.0f, 4.0f, // top-right
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 4.0f, // top-left
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            // right face (CCW)
            0.5f, -0.5f,  0.5f, 1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f, 4.0f, 0.0f, // bottom-right
            0.5f,  0.5f, -0.5f, 1.0f,  0.0f,  0.0f, 4.0f, 4.0f, // top-right
            0.5f,  0.5f, -0.5f, 1.0f,  0.0f,  0.0f, 4.0f, 4.0f, // top-right
            0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f, 0.0f, 4.0f, // top-left
            0.5f, -0.5f,  0.5f, 1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            // bottom face (CCW)
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 4.0f, 0.0f, // bottom-right
            0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f, 4.0f, 4.0f, // top-right
            0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f, 4.0f, 4.0f, // top-right
            -0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 4.0f, // top-left
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            // top face (CCW)
            -0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 4.0f, 0.0f, // bottom-right
            0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 4.0f, 4.0f, // top-right
            0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 4.0f, 4.0f, // top-right
            -0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 4.0f, // top-left
            -0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f, // bottom-left
    };

    // basic cube vertices - to be used for drawing walls and light cubes
    float wallVertices[] = {
            // positions                // normals              //texture coords
            // back face (CCW winding)
            0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  0.0f, 0.0f, // bottom-left
            -0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
            -0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            // front face (CCW winding)
            -0.5f, -0.5f,  0.5f, 0.0f,  0.0f,  1.0f,  0.0f, 0.0f, // bottom-left
            0.5f, -0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
            0.5f,  0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            0.5f,  0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -0.5f,  0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -0.5f, -0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face (CCW)
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // top-left
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            // right face (CCW)
            0.5f, -0.5f,  0.5f, 1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            0.5f,  0.5f, -0.5f, 1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
            0.5f,  0.5f, -0.5f, 1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
            0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // top-left
            0.5f, -0.5f,  0.5f, 1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            // bottom face (CCW)
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-right
            0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-right
            -0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            // top face (CCW)
            -0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
            0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
            -0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f, // bottom-left
    };

    // platform positions
    glm::vec3 platformPositions[] = {
            glm::vec3( -3.0f,  0.5f,  0.0f),
            glm::vec3( 1.5f, 0.0f, 0.5f)
    };

    // wall positions
    glm::vec3 wallPositions[] = {
            glm::vec3( -3.0f,  1.625f,  -2.525f),
            glm::vec3( -5.425f, 1.625f, -1.2f),
            glm::vec3( 2.5f, 1.125f, -2.025f),
            glm::vec3( 3.925f, 1.125f, 0.05f)
    };

    // stairs - position and angle for every step
    vector<pair<glm::vec3, float>> stairs = {
            {glm::vec3( -0.3f,  0.5f,  2.0f), 0.0f},
            {glm::vec3( -0.05f, 0.4f, 1.95f), 10.f},
            {glm::vec3( 0.2f,  0.3f,  1.9f), 20.f},
            {glm::vec3( 0.45f, 0.2f, 1.8f), 30.f}
    };

    // point light positions - light cubes
    glm::vec3 pointLightPositions[] = {
            glm::vec3( -0.2f,  0.6f,  2.6f),
            glm::vec3( 0.0f, 0.5f, 1.3f)
    };

    // spotlight positions
    glm::vec3 spotLightPositions[] = {
            glm::vec3( -4.95f,  2.375f,  -1.6f),
            glm::vec3( -4.8f,  2.375f,  -1.71f)
    };

    unsigned int platformVBO, platformVAO;
    glGenVertexArrays(1, &platformVAO);
    glGenBuffers(1, &platformVBO);

    glBindBuffer(GL_ARRAY_BUFFER, platformVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(platformVertices), platformVertices, GL_STATIC_DRAW);

    glBindVertexArray(platformVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int wallVBO, wallVAO;
    glGenVertexArrays(1, &wallVAO);
    glGenBuffers(1, &wallVBO);

    glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), wallVertices, GL_STATIC_DRAW);

    glBindVertexArray(wallVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // light cube VAO
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // load textures - using a utility function to keep the code more organized
    unsigned int diffuseMapPlatform1 = loadTexture("resources/textures/WoodFlooringAshSuperWhite_diffuse.jpg");
    unsigned int specularMapPlatform1 = loadTexture("resources/textures/WoodFlooringAshSuperWhite_specular.jpg");
    unsigned int diffuseMapPlatform2 = loadTexture("resources/textures/TilesBlackSlateSquare_diffuse.png");
    unsigned int specularMapPlatform2 = loadTexture("resources/textures/TilesBlackSlateSquare_specular.png");
    unsigned int diffuseMapWall1 = loadTexture("resources/textures/BricksReclaimedWhitewashedOffset_diffuse.png");
    unsigned int specularMapWall1 = loadTexture("resources/textures/BricksReclaimedWhitewashedOffset_specular.png");
    unsigned int diffuseMapWall2 = loadTexture("resources/textures/StuccoRoughCast2_diffuse.png");
    unsigned int specularMapWall2 = loadTexture("resources/textures/StuccoRoughCast_specular.png");
    unsigned int diffuseMapGlass = loadTexture("resources/textures/glass1_diffuse.png");
    unsigned int specularMapGlass = loadTexture("resources/textures/glass1_specular.png");

    // load models
    Model floorLampModel("resources/objects/FloorLamp/FloorLamp.obj");
    floorLampModel.SetShaderTextureNamePrefix("material.");


    // shader configuration
    platform1Shader.use();
    platform1Shader.setInt("material.diffuse", 0);
    platform1Shader.setInt("material.specular", 1);

    platform2Shader.use();
    platform2Shader.setInt("material.diffuse", 2);
    platform2Shader.setInt("material.specular", 3);

    wall1Shader.use();
    wall1Shader.setInt("material.diffuse", 4);
    wall1Shader.setInt("material.specular", 5);

    wall2Shader.use();
    wall2Shader.setInt("material.diffuse", 6);
    wall2Shader.setInt("material.specular", 7);

    stairsShader.use();
    stairsShader.setInt("material.diffuse", 8);
    stairsShader.setInt("material.specular", 9);


    // directional light settings
    glm::vec3 direction = glm::vec3(0.0f, -4.0f, -5.0f);
    glm::vec3 dirLightAmbient = glm::vec3(0.05f, 0.05f, 0.05f);
    glm::vec3 dirLightDiffuse = glm::vec3(0.4f, 0.4f, 0.4f);
    glm::vec3 dirLightSpecular = glm::vec3(0.5f, 0.5f, 0.5f);

    // point lights settings
    glm::vec3 pointLightAmbient = glm::vec3(0.05f, 0.05f, 0.05f);
    glm::vec3 pointLightDiffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    glm::vec3 pointLightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
    float pointLightConstant = 1.0f;
    float pointLightLinear = 0.09f;
    float pointLightQuadratic = 0.032f;

    // spotlights settings
    glm::vec3 spotLightDirection = glm::vec3(0.0f,  -1.0f,  0.0f);
    glm::vec3 spotLightAmbient = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 spotLightDiffuse = glm::vec3(0.6f, 0.6f, 0.6f);
    glm::vec3 spotLightSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
    float spotLightConstant = 1.0f;
    float spotLightLinear = 0.09f;
    float spotLightQuadratic = 0.032f;
    float cutOff = glm::cos(glm::radians(9.5f));
    float outerCutOff = glm::cos(glm::radians(55.0f));

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // enabling blending to achieve transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // render loop
    while (!glfwWindowShouldClose(window)) {

        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        glClearColor(0.1, 0.1, 0.1, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // view/projection matrices
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        // enabling face culling for platforms and walls
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);

        // =========================================== draw platforms ===========================================

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMapPlatform1);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMapPlatform1);
        // bind diffuse map
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, diffuseMapPlatform2);
        // bind specular map
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, specularMapPlatform2);

        glBindVertexArray(platformVAO);

        // ------------------------------------------- first platform -------------------------------------------
        platform1Shader.use();

        platform1Shader.setVec3("viewPos", camera.Position);
        platform1Shader.setFloat("material.shininess", 32.0f);

        // directional light
        platform1Shader.setVec3("dirLight.direction", direction);
        platform1Shader.setVec3("dirLight.ambient", dirLightAmbient);
        platform1Shader.setVec3("dirLight.diffuse", dirLightDiffuse);
        platform1Shader.setVec3("dirLight.specular", dirLightSpecular);

        // point light 1
        platform1Shader.setVec3("pointLights[0].position",
                                pointLightPositions[0] + glm::vec3(0.0f, 0.2 * sin(2 * glfwGetTime()), 0.0f));
        platform1Shader.setVec3("pointLights[0].ambient", pointLightAmbient);
        platform1Shader.setVec3("pointLights[0].diffuse", pointLightDiffuse);
        platform1Shader.setVec3("pointLights[0].specular", pointLightSpecular);
        platform1Shader.setFloat("pointLights[0].constant", pointLightConstant);
        platform1Shader.setFloat("pointLights[0].linear", pointLightLinear);
        platform1Shader.setFloat("pointLights[0].quadratic", pointLightQuadratic);
        // point light 2
        platform1Shader.setVec3("pointLights[1].position",
                                pointLightPositions[1] + glm::vec3(0.0f, 0.2 * sin(2 * glfwGetTime() + 1), 0.0f));
        platform1Shader.setVec3("pointLights[1].ambient", pointLightAmbient);
        platform1Shader.setVec3("pointLights[1].diffuse", pointLightDiffuse);
        platform1Shader.setVec3("pointLights[1].specular", pointLightSpecular);
        platform1Shader.setFloat("pointLights[1].constant", pointLightConstant);
        platform1Shader.setFloat("pointLights[1].linear", pointLightLinear);
        platform1Shader.setFloat("pointLights[1].quadratic", pointLightQuadratic);

        // spotlight 1
        platform1Shader.setVec3("spotLights[0].position", spotLightPositions[0]);
        platform1Shader.setVec3("spotLights[0].direction", spotLightDirection);
        platform1Shader.setVec3("spotLights[0].ambient", spotLightAmbient);
        platform1Shader.setVec3("spotLights[0].diffuse", spotLightDiffuse);
        platform1Shader.setVec3("spotLights[0].specular", spotLightSpecular);
        platform1Shader.setFloat("spotLights[0].constant", spotLightConstant);
        platform1Shader.setFloat("spotLights[0].linear", spotLightLinear);
        platform1Shader.setFloat("spotLights[0].quadratic", spotLightQuadratic);
        platform1Shader.setFloat("spotLights[0].cutOff", cutOff);
        platform1Shader.setFloat("spotLights[0].outerCutOff", outerCutOff);

        // spotlight 2
        platform1Shader.setVec3("spotLights[1].position", spotLightPositions[1]);
        platform1Shader.setVec3("spotLights[1].direction", spotLightDirection);
        platform1Shader.setVec3("spotLights[1].ambient", spotLightAmbient);
        platform1Shader.setVec3("spotLights[1].diffuse", spotLightDiffuse);
        platform1Shader.setVec3("spotLights[1].specular", spotLightSpecular);
        platform1Shader.setFloat("spotLights[1].constant", spotLightConstant);
        platform1Shader.setFloat("spotLights[1].linear", spotLightLinear);
        platform1Shader.setFloat("spotLights[1].quadratic", spotLightQuadratic);
        platform1Shader.setFloat("spotLights[1].cutOff", cutOff);
        platform1Shader.setFloat("spotLights[1].outerCutOff", outerCutOff);

        // view/projection transformations
        platform1Shader.setMat4("projection", projection);
        platform1Shader.setMat4("view", view);

        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, platformPositions[0]);
        model = glm::scale(model, glm::vec3(5.0f, 0.15f, 5.2f));
        platform1Shader.setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        // ------------------------------------------- second platform -------------------------------------------
        platform2Shader.use();

        platform2Shader.setVec3("viewPos", camera.Position);
        platform2Shader.setFloat("material.shininess", 32.0f);

        // directional light
        platform2Shader.setVec3("dirLight.direction", direction);
        platform2Shader.setVec3("dirLight.ambient", dirLightAmbient);
        platform2Shader.setVec3("dirLight.diffuse", dirLightDiffuse);
        platform2Shader.setVec3("dirLight.specular", dirLightSpecular);

        // point light 1
        platform2Shader.setVec3("pointLights[0].position",
                                pointLightPositions[0] + glm::vec3(0.0f, 0.2 * sin(2 * glfwGetTime()), 0.0f));
        platform2Shader.setVec3("pointLights[0].ambient", pointLightAmbient);
        platform2Shader.setVec3("pointLights[0].diffuse", pointLightDiffuse);
        platform2Shader.setVec3("pointLights[0].specular", pointLightSpecular);
        platform2Shader.setFloat("pointLights[0].constant", pointLightConstant);
        platform2Shader.setFloat("pointLights[0].linear", pointLightLinear);
        platform2Shader.setFloat("pointLights[0].quadratic", pointLightQuadratic);
        // point light 2
        platform2Shader.setVec3("pointLights[1].position",
                                pointLightPositions[1] + glm::vec3(0.0f, 0.2 * sin(2 * glfwGetTime() + 1), 0.0f));
        platform2Shader.setVec3("pointLights[1].ambient", pointLightAmbient);
        platform2Shader.setVec3("pointLights[1].diffuse", pointLightDiffuse);
        platform2Shader.setVec3("pointLights[1].specular", pointLightSpecular);
        platform2Shader.setFloat("pointLights[1].constant", pointLightConstant);
        platform2Shader.setFloat("pointLights[1].linear", pointLightLinear);
        platform2Shader.setFloat("pointLights[1].quadratic", pointLightQuadratic);

        // spotlight 1
        platform2Shader.setVec3("spotLights[0].position", spotLightPositions[0]);
        platform2Shader.setVec3("spotLights[0].direction", spotLightDirection);
        platform2Shader.setVec3("spotLights[0].ambient", spotLightAmbient);
        platform2Shader.setVec3("spotLights[0].diffuse", spotLightDiffuse);
        platform2Shader.setVec3("spotLights[0].specular", spotLightSpecular);
        platform2Shader.setFloat("spotLights[0].constant", spotLightConstant);
        platform2Shader.setFloat("spotLights[0].linear", spotLightLinear);
        platform2Shader.setFloat("spotLights[0].quadratic", spotLightQuadratic);
        platform2Shader.setFloat("spotLights[0].cutOff", cutOff);
        platform2Shader.setFloat("spotLights[0].outerCutOff", outerCutOff);

        // spotlight 2
        platform2Shader.setVec3("spotLights[1].position", spotLightPositions[1]);
        platform2Shader.setVec3("spotLights[1].direction", spotLightDirection);
        platform2Shader.setVec3("spotLights[1].ambient", spotLightAmbient);
        platform2Shader.setVec3("spotLights[1].diffuse", spotLightDiffuse);
        platform2Shader.setVec3("spotLights[1].specular", spotLightSpecular);
        platform2Shader.setFloat("spotLights[1].constant", spotLightConstant);
        platform2Shader.setFloat("spotLights[1].linear", spotLightLinear);
        platform2Shader.setFloat("spotLights[1].quadratic", spotLightQuadratic);
        platform2Shader.setFloat("spotLights[1].cutOff", cutOff);
        platform2Shader.setFloat("spotLights[1].outerCutOff", outerCutOff);

        // view/projection transformations
        platform2Shader.setMat4("projection", projection);
        platform2Shader.setMat4("view", view);

        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, platformPositions[1]);
        model = glm::scale(model, glm::vec3(5.0f, 0.15f, 5.2f));
        platform2Shader.setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        // =========================================== platforms drawn ===========================================

        // ============================================ draw models ==============================================
        modelShader.use();

        modelShader.setVec3("viewPos", camera.Position);
        modelShader.setFloat("material.shininess", 32.0f);

        // directional light
        modelShader.setVec3("dirLight.direction", direction);
        modelShader.setVec3("dirLight.ambient", dirLightAmbient);
        modelShader.setVec3("dirLight.diffuse", dirLightDiffuse);
        modelShader.setVec3("dirLight.specular", dirLightSpecular);

        // point light 1
        modelShader.setVec3("pointLights[0].position",
                                pointLightPositions[0] + glm::vec3(0.0f, 0.2 * sin(2 * glfwGetTime()), 0.0f));
        modelShader.setVec3("pointLights[0].ambient", pointLightAmbient);
        modelShader.setVec3("pointLights[0].diffuse", pointLightDiffuse);
        modelShader.setVec3("pointLights[0].specular", pointLightSpecular);
        modelShader.setFloat("pointLights[0].constant", pointLightConstant);
        modelShader.setFloat("pointLights[0].linear", pointLightLinear);
        modelShader.setFloat("pointLights[0].quadratic", pointLightQuadratic);
        // point light 2
        modelShader.setVec3("pointLights[1].position",
                                pointLightPositions[1] + glm::vec3(0.0f, 0.2 * sin(2 * glfwGetTime() + 1), 0.0f));
        modelShader.setVec3("pointLights[1].ambient", pointLightAmbient);
        modelShader.setVec3("pointLights[1].diffuse", pointLightDiffuse);
        modelShader.setVec3("pointLights[1].specular", pointLightSpecular);
        modelShader.setFloat("pointLights[1].constant", pointLightConstant);
        modelShader.setFloat("pointLights[1].linear", pointLightLinear);
        modelShader.setFloat("pointLights[1].quadratic", pointLightQuadratic);

        // spotlight 1
        modelShader.setVec3("spotLights[0].position", spotLightPositions[0]);
        modelShader.setVec3("spotLights[0].direction", spotLightDirection);
        modelShader.setVec3("spotLights[0].ambient", spotLightAmbient);
        modelShader.setVec3("spotLights[0].diffuse", spotLightDiffuse);
        modelShader.setVec3("spotLights[0].specular", spotLightSpecular);
        modelShader.setFloat("spotLights[0].constant", spotLightConstant);
        modelShader.setFloat("spotLights[0].linear", spotLightLinear);
        modelShader.setFloat("spotLights[0].quadratic", spotLightQuadratic);
        modelShader.setFloat("spotLights[0].cutOff", cutOff);
        modelShader.setFloat("spotLights[0].outerCutOff", outerCutOff);

        // spotlight 2
        modelShader.setVec3("spotLights[1].position", spotLightPositions[1]);
        modelShader.setVec3("spotLights[1].direction", spotLightDirection);
        modelShader.setVec3("spotLights[1].ambient", spotLightAmbient);
        modelShader.setVec3("spotLights[1].diffuse", spotLightDiffuse);
        modelShader.setVec3("spotLights[1].specular", spotLightSpecular);
        modelShader.setFloat("spotLights[1].constant", spotLightConstant);
        modelShader.setFloat("spotLights[1].linear", spotLightLinear);
        modelShader.setFloat("spotLights[1].quadratic", spotLightQuadratic);
        modelShader.setFloat("spotLights[1].cutOff", cutOff);
        modelShader.setFloat("spotLights[1].outerCutOff", outerCutOff);

        // view/projection transformations
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);

        // ------------------------------------------- floorLampModel -------------------------------------------
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-5.0f,  0.575f,  -1.8f));
        model = glm::rotate(model, glm::radians(40.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
        modelShader.setMat4("model", model);
        floorLampModel.Draw(modelShader);




        // ============================================ models drawn ==============================================

        // ============================================ draw light cubes ==========================================
        lightCubeShader.use();

        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);

        glBindVertexArray(lightCubeVAO);
        for (unsigned int i = 0; i < NUM_LIGHT_CUBES; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i] + glm::vec3(0.0f, 0.2 * sin(2 * glfwGetTime() + i), 0.0f));
            model = glm::scale(model, glm::vec3(0.1f));
            lightCubeShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // ============================================ light cubes drawn =========================================

        // =========================================== draw walls ================================================

        // bind diffuse map
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, diffuseMapWall1);
        // bind specular map
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, specularMapWall1);
        // bind diffuse map
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, diffuseMapWall2);
        // bind specular map
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, specularMapWall2);

        glBindVertexArray(wallVAO);

        // ------------------------------------------- 1st wall --------------------------------------------------
        wall1Shader.use();

        wall1Shader.setVec3("viewPos", camera.Position);
        wall1Shader.setFloat("material.shininess", 32.0f);

        // directional light
        wall1Shader.setVec3("dirLight.direction", direction);
        wall1Shader.setVec3("dirLight.ambient", dirLightAmbient);
        wall1Shader.setVec3("dirLight.diffuse", dirLightDiffuse);
        wall1Shader.setVec3("dirLight.specular", dirLightSpecular);

        // point light 1
        wall1Shader.setVec3("pointLights[0].position",
                            pointLightPositions[0] + glm::vec3(0.0f, 0.2 * sin(2 * glfwGetTime()), 0.0f));
        wall1Shader.setVec3("pointLights[0].ambient", pointLightAmbient);
        wall1Shader.setVec3("pointLights[0].diffuse", pointLightDiffuse);
        wall1Shader.setVec3("pointLights[0].specular", pointLightSpecular);
        wall1Shader.setFloat("pointLights[0].constant", pointLightConstant);
        wall1Shader.setFloat("pointLights[0].linear", pointLightLinear);
        wall1Shader.setFloat("pointLights[0].quadratic", pointLightQuadratic);
        // point light 2
        wall1Shader.setVec3("pointLights[1].position",
                            pointLightPositions[1] + glm::vec3(0.0f, 0.2 * sin(2 * glfwGetTime() + 1), 0.0f));
        wall1Shader.setVec3("pointLights[1].ambient", pointLightAmbient);
        wall1Shader.setVec3("pointLights[1].diffuse", pointLightDiffuse);
        wall1Shader.setVec3("pointLights[1].specular", pointLightSpecular);
        wall1Shader.setFloat("pointLights[1].constant", pointLightConstant);
        wall1Shader.setFloat("pointLights[1].linear", pointLightLinear);
        wall1Shader.setFloat("pointLights[1].quadratic", pointLightQuadratic);

        // spotlight 1
        wall1Shader.setVec3("spotLights[0].position", spotLightPositions[0]);
        wall1Shader.setVec3("spotLights[0].direction", spotLightDirection);
        wall1Shader.setVec3("spotLights[0].ambient", spotLightAmbient);
        wall1Shader.setVec3("spotLights[0].diffuse", spotLightDiffuse);
        wall1Shader.setVec3("spotLights[0].specular", spotLightSpecular);
        wall1Shader.setFloat("spotLights[0].constant", spotLightConstant);
        wall1Shader.setFloat("spotLights[0].linear", spotLightLinear);
        wall1Shader.setFloat("spotLights[0].quadratic", spotLightQuadratic);
        wall1Shader.setFloat("spotLights[0].cutOff", cutOff);
        wall1Shader.setFloat("spotLights[0].outerCutOff", outerCutOff);

        // spotlight 2
        wall1Shader.setVec3("spotLights[1].position", spotLightPositions[1]);
        wall1Shader.setVec3("spotLights[1].direction", spotLightDirection);
        wall1Shader.setVec3("spotLights[1].ambient", spotLightAmbient);
        wall1Shader.setVec3("spotLights[1].diffuse", spotLightDiffuse);
        wall1Shader.setVec3("spotLights[1].specular", spotLightSpecular);
        wall1Shader.setFloat("spotLights[1].constant", spotLightConstant);
        wall1Shader.setFloat("spotLights[1].linear", spotLightLinear);
        wall1Shader.setFloat("spotLights[1].quadratic", spotLightQuadratic);
        wall1Shader.setFloat("spotLights[1].cutOff", cutOff);
        wall1Shader.setFloat("spotLights[1].outerCutOff", outerCutOff);

        // view/projection transformations
        wall1Shader.setMat4("projection", projection);
        wall1Shader.setMat4("view", view);

        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, wallPositions[0]);
        model = glm::scale(model, glm::vec3(5.0f, 2.1f, 0.15f));
        wall1Shader.setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        // ------------------------------------------- 2nd wall --------------------------------------------------
        // uses the same shader as 1st wall
        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, wallPositions[1]);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(2.5f, 2.1f, 0.15f));
        wall1Shader.setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        // ------------------------------------------- 3rd wall --------------------------------------------------
        wall2Shader.use();

        wall2Shader.setVec3("viewPos", camera.Position);
        wall2Shader.setFloat("material.shininess", 32.0f);

        // directional light
        wall2Shader.setVec3("dirLight.direction", direction);
        wall2Shader.setVec3("dirLight.ambient", dirLightAmbient);
        wall2Shader.setVec3("dirLight.diffuse", dirLightDiffuse);
        wall2Shader.setVec3("dirLight.specular", dirLightSpecular);

        // point light 1
        wall2Shader.setVec3("pointLights[0].position",
                            pointLightPositions[0] + glm::vec3(0.0f, 0.2 * sin(2 * glfwGetTime()), 0.0f));
        wall2Shader.setVec3("pointLights[0].ambient", pointLightAmbient);
        wall2Shader.setVec3("pointLights[0].diffuse", pointLightDiffuse);
        wall2Shader.setVec3("pointLights[0].specular", pointLightSpecular);
        wall2Shader.setFloat("pointLights[0].constant", pointLightConstant);
        wall2Shader.setFloat("pointLights[0].linear", pointLightLinear);
        wall2Shader.setFloat("pointLights[0].quadratic", pointLightQuadratic);
        // point light 2
        wall2Shader.setVec3("pointLights[1].position",
                            pointLightPositions[1] + glm::vec3(0.0f, 0.2 * sin(2 * glfwGetTime() + 1), 0.0f));
        wall2Shader.setVec3("pointLights[1].ambient", pointLightAmbient);
        wall2Shader.setVec3("pointLights[1].diffuse", pointLightDiffuse);
        wall2Shader.setVec3("pointLights[1].specular", pointLightSpecular);
        wall2Shader.setFloat("pointLights[1].constant", pointLightConstant);
        wall2Shader.setFloat("pointLights[1].linear", pointLightLinear);
        wall2Shader.setFloat("pointLights[1].quadratic", pointLightQuadratic);

        // spotlight 1
        wall2Shader.setVec3("spotLights[0].position", spotLightPositions[0]);
        wall2Shader.setVec3("spotLights[0].direction", spotLightDirection);
        wall2Shader.setVec3("spotLights[0].ambient", spotLightAmbient);
        wall2Shader.setVec3("spotLights[0].diffuse", spotLightDiffuse);
        wall2Shader.setVec3("spotLights[0].specular", spotLightSpecular);
        wall2Shader.setFloat("spotLights[0].constant", spotLightConstant);
        wall2Shader.setFloat("spotLights[0].linear", spotLightLinear);
        wall2Shader.setFloat("spotLights[0].quadratic", spotLightQuadratic);
        wall2Shader.setFloat("spotLights[0].cutOff", cutOff);
        wall2Shader.setFloat("spotLights[0].outerCutOff", outerCutOff);

        // spotlight 2
        wall2Shader.setVec3("spotLights[1].position", spotLightPositions[1]);
        wall2Shader.setVec3("spotLights[1].direction", spotLightDirection);
        wall2Shader.setVec3("spotLights[1].ambient", spotLightAmbient);
        wall2Shader.setVec3("spotLights[1].diffuse", spotLightDiffuse);
        wall2Shader.setVec3("spotLights[1].specular", spotLightSpecular);
        wall2Shader.setFloat("spotLights[1].constant", spotLightConstant);
        wall2Shader.setFloat("spotLights[1].linear", spotLightLinear);
        wall2Shader.setFloat("spotLights[1].quadratic", spotLightQuadratic);
        wall2Shader.setFloat("spotLights[1].cutOff", cutOff);
        wall2Shader.setFloat("spotLights[1].outerCutOff", outerCutOff);

        // view/projection transformations
        wall2Shader.setMat4("projection", projection);
        wall2Shader.setMat4("view", view);

        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, wallPositions[2]);
        model = glm::scale(model, glm::vec3(3.0f, 2.1f, 0.15f));
        wall2Shader.setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        // ------------------------------------------- 4th wall --------------------------------------------------
        // uses the same shader as 3rd wall
        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, wallPositions[3]);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(4.0f, 2.1f, 0.15f));
        wall2Shader.setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        // =========================================== walls drawn ===============================================

        // I don't want face culling for anything other than platforms and walls
        glDisable(GL_CULL_FACE);

        // =========================================== draw glass stairs =========================================
        // using wallVBO & wallVAO

        // bind diffuse map
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, diffuseMapGlass);
        // bind specular map
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, specularMapGlass);

        stairsShader.use();

        stairsShader.setVec3("viewPos", camera.Position);
        stairsShader.setFloat("material.shininess", 32.0f);

        // directional light
        stairsShader.setVec3("dirLight.direction", direction);
        stairsShader.setVec3("dirLight.ambient", dirLightAmbient);
        stairsShader.setVec3("dirLight.diffuse", dirLightDiffuse);
        stairsShader.setVec3("dirLight.specular", dirLightSpecular);

        // point light 1
        stairsShader.setVec3("pointLights[0].position",
                             pointLightPositions[0] + glm::vec3(0.0f, 0.2 * sin(2 * glfwGetTime()), 0.0f));
        stairsShader.setVec3("pointLights[0].ambient", pointLightAmbient);
        stairsShader.setVec3("pointLights[0].diffuse", pointLightDiffuse);
        stairsShader.setVec3("pointLights[0].specular", pointLightSpecular);
        stairsShader.setFloat("pointLights[0].constant", pointLightConstant);
        stairsShader.setFloat("pointLights[0].linear", pointLightLinear);
        stairsShader.setFloat("pointLights[0].quadratic", pointLightQuadratic);
        // point light 2
        stairsShader.setVec3("pointLights[1].position",
                             pointLightPositions[1] + glm::vec3(0.0f, 0.2 * sin(2 * glfwGetTime() + 1), 0.0f));
        stairsShader.setVec3("pointLights[1].ambient", pointLightAmbient);
        stairsShader.setVec3("pointLights[1].diffuse", pointLightDiffuse);
        stairsShader.setVec3("pointLights[1].specular", pointLightSpecular);
        stairsShader.setFloat("pointLights[1].constant", pointLightConstant);
        stairsShader.setFloat("pointLights[1].linear", pointLightLinear);
        stairsShader.setFloat("pointLights[1].quadratic", pointLightQuadratic);

        // spotlight 1
        stairsShader.setVec3("spotLights[0].position", spotLightPositions[0]);
        stairsShader.setVec3("spotLights[0].direction", spotLightDirection);
        stairsShader.setVec3("spotLights[0].ambient", spotLightAmbient);
        stairsShader.setVec3("spotLights[0].diffuse", spotLightDiffuse);
        stairsShader.setVec3("spotLights[0].specular", spotLightSpecular);
        stairsShader.setFloat("spotLights[0].constant", spotLightConstant);
        stairsShader.setFloat("spotLights[0].linear", spotLightLinear);
        stairsShader.setFloat("spotLights[0].quadratic", spotLightQuadratic);
        stairsShader.setFloat("spotLights[0].cutOff", cutOff);
        stairsShader.setFloat("spotLights[0].outerCutOff", outerCutOff);

        // spotlight 2
        stairsShader.setVec3("spotLights[1].position", spotLightPositions[1]);
        stairsShader.setVec3("spotLights[1].direction", spotLightDirection);
        stairsShader.setVec3("spotLights[1].ambient", spotLightAmbient);
        stairsShader.setVec3("spotLights[1].diffuse", spotLightDiffuse);
        stairsShader.setVec3("spotLights[1].specular", spotLightSpecular);
        stairsShader.setFloat("spotLights[1].constant", spotLightConstant);
        stairsShader.setFloat("spotLights[1].linear", spotLightLinear);
        stairsShader.setFloat("spotLights[1].quadratic", spotLightQuadratic);
        stairsShader.setFloat("spotLights[1].cutOff", cutOff);
        stairsShader.setFloat("spotLights[1].outerCutOff", outerCutOff);

        // view/projection transformations
        stairsShader.setMat4("projection", projection);
        stairsShader.setMat4("view", view);

        // steps need to be sorted because of their transparency - if rendered differently
        // some steps may not be visible through the other ones
        std::sort(stairs.begin(), stairs.end(),
                  [cameraPosition = camera.Position](const pair<glm::vec3, float>& a, const pair<glm::vec3, float>& b) {
                      float d1 = glm::distance(a.first, cameraPosition);
                      float d2 = glm::distance(b.first, cameraPosition);
                      return d1 > d2;
        });

        for (const pair<glm::vec3, float>& step : stairs) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, step.first);
            model = glm::rotate(model, glm::radians(step.second), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.25f, 0.05f, 0.75f));
            stairsShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // =========================================== glass stairs drawn =========================================


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}


void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {

}

// utility function for loading a 2D texture from file
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
