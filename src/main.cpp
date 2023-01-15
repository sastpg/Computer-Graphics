#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include "shader.h"
#include "model.h"
#include "base/texture.h"
#include "base/skybox.h"
#include "base/camera.h"
#include "plymodel.h"
#include "diymodel.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
//#include "GeometricSolid.h"
//#include "partical_life.h"

#define SCR_WIDTH 1920
#define SCR_HEIGHT 1080
#define NumberOfStone 3
#define NumOfGeometrics 20

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
int mode;

// parameters
bool light = false;
bool collision = false;
bool rock_visible = true;
bool is_bomb = false;
int rateCount = 0;
double rate = 0.01;
float bombx = 0.5, bomby = 3;
// material
std::vector<vector<float> > material4 = { {0.25, 0.20725, 0.20725}, {169.0 / 255, 169.0 / 255, 169.0 / 255}, {0.296648, 0.296648, 0.296648} };
std::vector<vector<float> > material3 = { {0.19225,0.19225,0.19225}, {0.50754,0.50754,0.50754}, {0.508273,0.508273,0.508273} };
std::vector<vector<float> > material2 = { {0.192250f, 0.192250f, 0.192250f}, {0.667540f, 0.557540f, 0.337540f}, {0.508273f, 0.508273f, 0.508273f} };
std::vector<vector<float> > material1 = { {0.247250f, 0.199500f, 0.074500f}, {0.751640f, 0.606480f, 0.226480f}, {0.628281f, 0.555802f, 0.556065f} };
std::vector<float>Shininess = { 11.264, 51.2, 61.2, 171.200001f };

void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void saveimage(const string& file, int width, int height);
float inter(float a, float b, float time, float interval);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open GLFW Window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HelloWorld", NULL, NULL);
    if (window == NULL) {
        std::cout << "Open Windows failed!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // ------------------------------------------
    Shader shader("media/shader/normal_mapping.vs", "media/shader/normal_mapping.fs");
    Shader skyboxShader("media/shader/skybox1.vs", "media/shader/skybox1.fs");
    SkyBox skybox1(1);

    Shader modelShader("media/shader/light_casters.vs", "media/shader/light_casters.fs");
    Model skull("media/obj/skull/skull.obj");
	Model skull1("media/obj/diy/cube.obj");
	Model vase("media/obj/vase/HSM0037.obj");
    Shader shaderBall("media/shader/pbr.vs", "media/shader/pbr.fs");
    Shader skyboxShader1("media/shader/skybox2.vs", "media/shader/skybox2.fs");
    SkyBox skybox2(2);
    Shader floorShader("media/shader/blending.vs", "media/shader/blending.fs");
    Shader lightingShader("media/shader/light_casters.vs", "media/shader/light_casters.fs");
    Shader reconShader("media/shader/recon.vs", "media/shader/recon.fs");
    Shader lightingShader1("media/shader/light_casters1.vs", "media/shader/light_casters1.fs");
    
	PlyModel* happy_p1, * happy_p2, * happy_p3, * happy_p4;
	char name1[] = "media/obj/happy_recon/happy_vrip.ply";
	char name2[] = "media/obj/happy_recon/happy_vrip_res2.ply";
	char name3[] = "media/obj/happy_recon/happy_vrip_res3.ply";
	char name4[] = "media/obj/happy_recon/happy_vrip_res4.ply";
	PlyModel happy_recon1_0(name4, 0);
	happy_p1 = new PlyModel(name1, true);
	happy_p2 = new PlyModel(name2, true);
	happy_p3 = new PlyModel(name3, true);
	happy_p4 = new PlyModel(name4, true);

    // ----------------------------------------------
    floorShader.use();
    floorShader.setInt("texture1", 0);

    shaderBall.use();
    shaderBall.setInt("albedoMap", 0);
    shaderBall.setInt("normalMap", 1);
    shaderBall.setInt("metallicMap", 2);
    shaderBall.setInt("roughnessMap", 3);
    shaderBall.setInt("aoMap", 4);

    // shader configuration
    // --------------------
    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);

    Model rock("media/obj/rock/rock.obj");
    Model bomb("media/obj/Bomb/Bomb.obj");
    //Model test("resources/objects/happy_recon/happy_vrip.obj");

    // load PBR material textures
    // --------------------------
    unsigned int albedo = loadTexture("media/texture/pbr/rusted_iron/jin.png");
    unsigned int albedo1 = loadTexture("media/texture/pbr/rusted_iron/mu.png");
    unsigned int albedo2 = loadTexture("media/texture/pbr/rusted_iron/shui.png");
    unsigned int albedo3 = loadTexture("media/texture/pbr/rusted_iron/huo.png");
    unsigned int albedo4 = loadTexture("media/texture/pbr/rusted_iron/tu.png");
    unsigned int normal = loadTexture("media/texture/pbr/rusted_iron/normal.png");
    unsigned int metallic = loadTexture("media/texture/pbr/rusted_iron/metallic.png");
    unsigned int roughness = loadTexture("media/texture/pbr/rusted_iron/roughness.png");
    unsigned int ao = loadTexture("media/texture/pbr/rusted_iron/ao.png");

    // load textures
    // -------------
    unsigned int diffuseMap = loadTexture("media/texture/book/book.jpg");
    unsigned int normalMap = loadTexture("media/texture/book/book_normal1.jpg");
    unsigned int diffuseMap1 = loadTexture("media/texture/book/bookLeft.png");
    unsigned int normalMap1 = loadTexture("media/texture/book/bookLeft_normal.jpg");
    unsigned int diffuseMap2 = loadTexture("media/texture/book/bookBottom.png");
    unsigned int normalMap2 = loadTexture("media/texture/book/bookBottom_normal.jpg");

    //floor
    unsigned int floorTexture = loadTexture("media/texture/skybox2/metal.png");

    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("diffuseMap", 0);
    shader.setInt("normalMap", 1);

    // lighting info
    // -------------
    glm::vec3 lightPos(0.5f, 0.5f, 1.3f);

    // shader configuration
// --------------------
    lightingShader1.use();
    lightingShader1.setInt("material.diffuse", 0);
    lightingShader1.setInt("material.specular", 1);

    // shader configuration
// --------------------
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);
    //float fogColor[] = { 1,1,1,1 };

        // lights
    // ------
    glm::vec3 lightPositions[] = {
        glm::vec3(5.0f, 5.0f, 5.0f),
    };
    glm::vec3 lightColors[] = {
        glm::vec3(150.0f, 150.0f, 150.0f),
    };
    int nrRows = 7;
    int nrColumns = 7;
    float spacing = 2.5;

    // initialize static shader uniforms before rendering
    // --------------------------------------------------
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    shaderBall.use();
    shaderBall.setMat4("projection", projection);

    float startTime = glfwGetTime();
    float time;
    float alpha = 0;
    float time1 = 0;

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float planeVertices[] = {
        // positions          // texture Coords 
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };

    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    unsigned int amount = 100;
    glm::mat4* modelMatrices;
    modelMatrices = new glm::mat4[amount];
    srand(glfwGetTime()); // initialize random seed	
    float radius = 10.0;
    float offset = 2.5f;
    float x = 0, y = -0.5, z = -1;
    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        x += 0.1;
        if (i % 10 == 0)
        {
            y += 0.1;
            x = 0;
        }
        model = glm::translate(model, glm::vec3(x, y, z));
        float scale = (rand() % 20) / 100.0f + 0.05;
        model = glm::scale(model, glm::vec3(scale));
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));
        modelMatrices[i] = model;
    }

	// render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        time = currentFrame - startTime;

        if (time < 15)
            mode = 1;
        else
            mode = 2;

        // input
        // -----
        processInput(window);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

		if (mode == 1)
		{
			if (time > 12 && time < 15)
				camera.ProcessKeyboard(FORWARD, deltaTime);
			// configure view/projection matrices
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
			shader.use();
			shader.setMat4("projection", projection);
			shader.setMat4("view", view);
			// render normal-mapped quad
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
			model = glm::rotate(model, glm::radians((float)glfwGetTime() * -40.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
			shader.setMat4("model", model);
			shader.setVec3("viewPos", camera.Position);
			shader.setVec3("lightPos", lightPos);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normalMap);
			renderQuad();

			// render normal-mapped quad
			model = glm::mat4(1.0f);
			model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
			model = glm::rotate(model, glm::radians((float)glfwGetTime() * -40.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.1f));
			shader.setMat4("model", model);
			shader.setVec3("viewPos", camera.Position);
			shader.setVec3("lightPos", lightPos);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normalMap);
			renderQuad();

			// render normal-mapped quad
			model = glm::mat4(1.0f);
			model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
			model = glm::rotate(model, glm::radians((float)glfwGetTime() * -40.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
			model = glm::translate(model, glm::vec3(-0.44f, 0.0f, -0.05f)); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(90.0f), glm::normalize(glm::vec3(0.0, 1.0, 0.0))); // rotate the quad to show normal mapping from multiple directions
			shader.setMat4("model", model);
			shader.setVec3("viewPos", camera.Position);
			shader.setVec3("lightPos", lightPos);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normalMap1);
			renderQuad1();

			// render normal-mapped quad
			model = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians((float)glfwGetTime() * -40.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
			model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
			model = glm::translate(model, glm::vec3(0.44f, 0.0f, -0.05f)); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(90.0f), glm::normalize(glm::vec3(0.0, 1.0, 0.0))); // rotate the quad to show normal mapping from multiple directions
			shader.setMat4("model", model);
			shader.setVec3("viewPos", camera.Position);
			shader.setVec3("lightPos", lightPos);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap2);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normalMap2);
			renderQuad1();

			// render normal-mapped quad
			model = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians((float)glfwGetTime() * -40.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
			model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
			model = glm::translate(model, glm::vec3(0.0f, 0.64f, -0.05f)); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0))); // rotate the quad to show normal mapping from multiple directions
			shader.setMat4("model", model);
			shader.setVec3("viewPos", camera.Position);
			shader.setVec3("lightPos", lightPos);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap2);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normalMap2);
			renderQuad2();

			// render normal-mapped quad
			model = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians((float)glfwGetTime() * -40.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
			model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
			model = glm::translate(model, glm::vec3(0.0f, -0.64f, -0.05f)); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0))); // rotate the quad to show normal mapping from multiple directions
			shader.setMat4("model", model);
			shader.setVec3("viewPos", camera.Position);
			shader.setVec3("lightPos", lightPos);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap2);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normalMap2);
			renderQuad2();

			// draw ball
			shaderBall.use();
			view = camera.GetViewMatrix();
			shaderBall.setMat4("view", view);
			shaderBall.setVec3("camPos", camera.Position);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, albedo);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normal);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, metallic);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, roughness);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, ao);
			if (alpha < 1)
				alpha += 0.00015;
			//ball1
			model = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians((float)glfwGetTime() * 40.0f), glm::normalize(glm::vec3(0.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
			model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians((float)glfwGetTime() * 40.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
			model = glm::scale(model, glm::vec3(0.1f * alpha, 0.1f * alpha, 0.1f * alpha));
			shaderBall.setMat4("model", model);
			renderSphere();
			//ball2
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, albedo1);
			model = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians((float)glfwGetTime() * 40.0f), glm::normalize(glm::vec3(0.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
			model = glm::translate(model, glm::vec3(-0.95f, 0.31f, 0.0f));
			model = glm::rotate(model, glm::radians((float)glfwGetTime() * 40.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
			model = glm::scale(model, glm::vec3(0.1f * alpha, 0.1f * alpha, 0.1f * alpha));
			shaderBall.setMat4("model", model);
			renderSphere();
			//ball3
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, albedo2);
			model = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians((float)glfwGetTime() * 40.0f), glm::normalize(glm::vec3(0.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
			model = glm::translate(model, glm::vec3(0.95f, 0.31f, 0.0f));
			model = glm::rotate(model, glm::radians((float)glfwGetTime() * 40.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
			model = glm::scale(model, glm::vec3(0.1f * alpha, 0.1f * alpha, 0.1f * alpha));
			shaderBall.setMat4("model", model);
			renderSphere();
			//ball4
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, albedo3);
			model = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians((float)glfwGetTime() * 40.0f), glm::normalize(glm::vec3(0.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
			model = glm::translate(model, glm::vec3(0.59f, -0.81f, 0.0f));
			model = glm::rotate(model, glm::radians((float)glfwGetTime() * 40.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
			model = glm::scale(model, glm::vec3(0.1f * alpha, 0.1f * alpha, 0.1f * alpha));
			shaderBall.setMat4("model", model);
			renderSphere();
			//ball5
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, albedo4);
			model = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians((float)glfwGetTime() * 40.0f), glm::normalize(glm::vec3(0.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
			model = glm::translate(model, glm::vec3(-0.59f, -0.81f, 0.0f));
			model = glm::rotate(model, glm::radians((float)glfwGetTime() * 40.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
			model = glm::scale(model, glm::vec3(0.1f * alpha, 0.1f * alpha, 0.1f * alpha));
			shaderBall.setMat4("model", model);
			renderSphere();
			// render light source (simply re-render sphere at light positions)
			// this looks a bit off as we use the same shader, but it'll make their positions obvious and 
			// keeps the codeprint small.
			for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
			{
				glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
				newPos = lightPositions[i];
				shaderBall.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
				shaderBall.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

				model = glm::mat4(1.0f);
				model = glm::translate(model, newPos);
				model = glm::scale(model, glm::vec3(0.5f));
				shaderBall.setMat4("model", model);
				//renderSphere();
			}

			//draw skybox at last
			skyboxShader.use();
			view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_WIDTH, 0.1f, 100.0f);
			skyboxShader.setMat4("view", view);
			skyboxShader.setMat4("projection", projection);
			if (time < 5)
				skyboxShader.setFloat("u_time", 0);
			else
				skyboxShader.setFloat("u_time", time - 5);
			skybox1.draw();

		}

		if (mode == 2)
		{
			if (time > 15 && time < 18)
				camera.ProcessKeyboard(BACKWARD, deltaTime);

			lightingShader.use();
			lightingShader.setVec3("light.position", camera.Position);
			lightingShader.setVec3("light.direction", camera.Front);
			lightingShader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
			lightingShader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
			lightingShader.setVec3("viewPos", camera.Position);
			if (!light)
			{
				lightingShader.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);
				lightingShader.setVec3("light.diffuse", 0.1f, 0.1f, 0.1f);
				lightingShader.setVec3("light.specular", 0.1f, 0.1f, 0.1f);
				lightingShader.setFloat("light.constant", 1.0f);
				lightingShader.setFloat("light.linear", 0.09f);
				lightingShader.setFloat("light.quadratic", 0.032f);
			}
			else
			{
				lightingShader.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);
				lightingShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
				lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
				lightingShader.setFloat("light.constant", 1.0f);
				lightingShader.setFloat("light.linear", 0.09f);
				lightingShader.setFloat("light.quadratic", 0.032f);
			}
			lightingShader.setFloat("material.shininess", 2.0f);
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			view = camera.GetViewMatrix();
			lightingShader.setMat4("projection", projection);
			lightingShader.setMat4("view", view);
			model = glm::mat4(1.0f);
			lightingShader.setMat4("model", model);

			// skull right
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f)); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(-90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
			model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));	// it's a bit too big for our scene, so scale it down
			lightingShader.setMat4("model", model);
			skull.draw(lightingShader);
			// skull left
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-2.0f, -0.5f, 0.0f));
			model = glm::rotate(model, glm::radians(-90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
			model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));	// it's a bit too big for our scene, so scale it down
			lightingShader.setMat4("model", model);
			skull.draw(lightingShader);
			// skull top
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-1.0f, -0.5f, -1.6f));
			model = glm::rotate(model, glm::radians(-90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
			model = glm::rotate(model, glm::radians(90.0f), glm::normalize(glm::vec3(0.0, 0.0, 1.0)));
			model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));	// it's a bit too big for our scene, so scale it down
			lightingShader.setMat4("model", model);
			skull.draw(lightingShader);

			// vase
			//model = glm::mat4(1.0f);
			//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.8f)); // translate it down so it's at the center of the scene
			//model = glm::rotate(model, glm::radians(-90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
			//model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));	// it's a bit too big for our scene, so scale it down
			//lightingShader.setMat4("model", model);
			//vase.draw(lightingShader);
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.8f)); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(-90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
			model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));	// it's a bit too big for our scene, so scale it down
			lightingShader.setMat4("model", model);
			skull1.draw(lightingShader);

			//GeometricSolid* cube[NumOfGeometrics];
			// 没有爆炸的时候，显示rock
			if (!collision)
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
				for (unsigned int i = 0; i < amount; i++)
				{
					lightingShader.setMat4("model", modelMatrices[i]);
					rock.draw(lightingShader);
				}
			}

			//bombing rock
			// 炸弹碰到了，此时还应该显示rock爆炸画面
			if (collision && rock_visible)
			{
				rateCount++;
				if ((rateCount % 500 == 0) && rate > 0.001)
				{
					rate -= 0.001;
				}
				if (rateCount == 600)
				{
					rock_visible = false;
					light = false;
					time1 = currentFrame;
				}
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
				for (unsigned int i = 0; i < amount; i++)
				{
					glm::mat4 model = glm::mat4(1.0f);
					modelMatrices[i] = glm::translate(modelMatrices[i], glm::vec3((float)glfwGetTime() * rate, 0, 0));
				}
				for (unsigned int i = 0; i < amount; i++)
				{
					lightingShader.setMat4("model", modelMatrices[i]);
					rock.draw(lightingShader);
				}
			}
			// 爆炸，并且是第一次，画炸弹，判断是否碰到
			lightingShader.setFloat("material.shininess", 32.0f);
			if (is_bomb && !collision)
			{
				model = glm::mat4(1.0f);
				if (bomby > 0.5)
					bomby -= (float)glfwGetTime() * 0.0005;
				if (bomby < 0.5)
				{
					collision = true;
				}
				model = glm::translate(model, glm::vec3(bombx, bomby, -1));
				lightingShader.setMat4("model", model);
				bomb.draw(lightingShader);
			}

			if (!rock_visible) {
				lightingShader1.use();
				lightingShader1.setVec3("light.position", camera.Position);
				lightingShader1.setVec3("light.direction", camera.Front);
				lightingShader1.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
				lightingShader1.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
				lightingShader1.setVec3("viewPos", camera.Position);

				if (!light)
				{
					// light properties
					lightingShader1.setVec3("light.ambient", 0.0f, 0.0f, 0.0f);
					lightingShader1.setVec3("light.diffuse", 0.0f, 0.0f, 0.0f);
					lightingShader1.setVec3("light.specular", 0.0f, 0.0f, 0.0f);
					lightingShader1.setFloat("light.constant", 0.0f);
					lightingShader1.setFloat("light.linear", 0.0f);
					lightingShader1.setFloat("light.quadratic", 0.0f);
					// material properties
					lightingShader1.setFloat("material.shininess", 0.0f);
					// view/projection transformations
					glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
					glm::mat4 view = camera.GetViewMatrix();
					lightingShader1.setMat4("projection", projection);
					lightingShader1.setMat4("view", view);
					// world transformation
					glm::mat4 model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(0.0f, -1.0f, -2.0f));
					model = glm::scale(model, glm::vec3(10.0f));
					lightingShader1.setMat4("model", model);
					happy_recon1_0.Draw();
					time1 = currentFrame;
				}
				else
				{
					lightingShader1.setVec3("light.ambient", 0.0f, 0.0f, 0.0f);
					lightingShader1.setVec3("light.diffuse", 1.0f, 0.0f, 0.0f);
					lightingShader1.setVec3("light.specular", 0.4f, 0.4f, 0.4f);
					lightingShader1.setFloat("light.constant", 1.0f);
					lightingShader1.setFloat("light.linear", 0.09f);
					lightingShader1.setFloat("light.quadratic", 0.032f);

					// material properties
					lightingShader1.setFloat("material.shininess", 32.0f);

					// view/projection transformations
					glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
					glm::mat4 view = camera.GetViewMatrix();
					lightingShader1.setMat4("projection", projection);
					lightingShader1.setMat4("view", view);

					// world transformation
					glm::mat4 model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(0.0f, -1.0f, -2.0f));
					model = glm::scale(model, glm::vec3(10.0f));
					lightingShader1.setMat4("model", model);

					float timing = currentFrame - time1;
					float cuts[3] = { 3,6,9 };
					if (happy_p1 && timing > cuts[2])
					{
						float time = (timing - cuts[2]) > 3 ? 3 : (timing - cuts[2]);
						float interval = cuts[2] - cuts[1];
						lightingShader1.setVec3("light.ambient", inter(material2[0][0], material1[0][0], time, interval), inter(material2[0][1], material1[0][1], time, interval), inter(material2[0][2], material1[0][2], time, interval));
						lightingShader1.setVec3("light.diffuse", inter(material2[1][0], material1[1][0], time, interval), inter(material2[1][1], material1[1][1], time, interval), inter(material2[1][2], material1[1][2], time, interval));
						lightingShader1.setVec3("light.specular", inter(material2[2][0], material1[2][0], time, interval), inter(material2[2][1], material1[2][1], time, interval), inter(material2[2][2], material2[2][2], time, interval));
						lightingShader1.setFloat("material.shininess", inter(Shininess[3], Shininess[2], time, interval));
						if (happy_p1->needInit())
							happy_p1->afterMultiInit();
						happy_p1->Draw();
					}
					else if (happy_p2 && timing > cuts[1])
					{
						float time = (timing - cuts[1]);
						float interval = cuts[1] - cuts[0];
						lightingShader1.setVec3("light.ambient", inter(material3[0][0], material2[0][0], time, interval), inter(material3[0][1], material2[0][1], time, interval), inter(material3[0][2], material2[0][2], time, interval));
						lightingShader1.setVec3("light.diffuse", inter(material3[1][0], material2[1][0], time, interval), inter(material3[1][1], material2[1][1], time, interval), inter(material3[1][2], material2[1][2], time, interval));
						lightingShader1.setVec3("light.specular", inter(material3[2][0], material2[2][0], time, interval), inter(material3[2][1], material2[2][1], time, interval), inter(material3[2][2], material3[2][2], time, interval));
						lightingShader1.setFloat("material.shininess", inter(Shininess[2], Shininess[1], time, interval));
						if (happy_p2->needInit())
							happy_p2->afterMultiInit();
						happy_p2->Draw();
					}
					else if (happy_p3 && timing > cuts[0])
					{
						float time = (timing - cuts[0]);
						float interval = cuts[0] - 0;
						lightingShader1.setVec3("light.ambient", inter(material4[0][0], material3[0][0], time, interval), inter(material4[0][1], material3[0][1], time, interval), inter(material4[0][2], material3[0][2], time, interval));
						lightingShader1.setVec3("light.diffuse", inter(material4[1][0], material3[1][0], time, interval), inter(material4[1][1], material3[1][1], time, interval), inter(material4[1][2], material3[1][2], time, interval));
						lightingShader1.setVec3("light.specular", inter(material4[2][0], material3[2][0], time, interval), inter(material4[2][1], material3[2][1], time, interval), inter(material4[2][2], material3[2][2], time, interval));
						lightingShader1.setFloat("material.shininess", inter(Shininess[3], Shininess[2], time, interval));
						if (happy_p3->needInit())
							happy_p3->afterMultiInit();
						happy_p3->Draw();
					}
					else if (happy_p4)
					{
						lightingShader1.setVec3("light.ambient", material4[0][0], material4[0][1], material4[0][2]);
						lightingShader1.setVec3("light.diffuse", material4[1][0], material4[1][1], material4[1][2]);
						lightingShader1.setVec3("light.specular", material4[2][0], material4[2][1], material4[2][2]);
						lightingShader1.setFloat("material.shininess", Shininess[3]);
						if (happy_p4->needInit())
							happy_p4->afterMultiInit();
						happy_p4->Draw();
					}
				}
			}

			// ------
			floorShader.use();
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			floorShader.setMat4("projection", projection);
			floorShader.setMat4("view", view);
			if (light == 0)
			{
				floorShader.setVec3("light.ambient", 0.3f, 0.3f, 0.3f);
			}
			else
			{
				floorShader.setVec3("light.ambient", 0.5f, 0.5f, 0.5f);
			}

			glBindVertexArray(planeVAO);
			glBindTexture(GL_TEXTURE_2D, floorTexture);
			model = glm::mat4(1.0f);
			floorShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			//diymodel.Draw(camera, shader, lightPos);

			skyboxShader1.use();
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			//view = camera.GetViewMatrix();
			view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
			skyboxShader1.setMat4("view", view);
			skyboxShader1.setMat4("projection", projection);
			if (light == 0)
			{
				skyboxShader1.setVec3("light.ambient", 0.4f, 0.4f, 0.4f);
			}
			else
			{
				skyboxShader1.setVec3("light.ambient", 1.0f, 1.0f, 1.0f);
			}
			skybox2.draw();
		}

		glDepthFunc(GL_LESS); // set depth function back to default

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void saveimage(const string& file, int width, int height)
{
	std::string suffix = file.substr(file.find_last_of('.') + 1);
	int comp = 3;
	enum TextureType
	{
		PNG, JPG, BMP
	};

	TextureType type;
	if (!suffix.compare("png"))
	{
		comp = 4;
		type = PNG;
	}
	else if (!suffix.compare("jpg"))
		type = JPG;
	else if (!suffix.compare("bmp"))
		type = BMP;

	unsigned char* data = new unsigned char[width * height * comp];

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, &data[0]);

	unsigned char* newdata = new unsigned char[width * height * comp];

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width * 3; j++)
			newdata[i * width * comp + j] = data[width * (height - i - 1) * comp + j];
	}

	switch (type)
	{
	case PNG:
		stbi_write_png(file.c_str(), width, height, comp, newdata, width * 4);
		break;
	case JPG:
		stbi_write_jpg(file.c_str(), width, height, comp, newdata, 100);
		break;
	case BMP:
		stbi_write_bmp(file.c_str(), width, height, comp, newdata);
		break;
	default:
		std::cout << "save texture failed" << std::endl;
		break;
	}
}

void processInput(GLFWwindow* window)
{
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
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		light = true;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		is_bomb = true;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		saveimage("screenshot.jpg", SCR_WIDTH, SCR_HEIGHT);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
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
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

float inter(float a, float b, float time, float interval)
{
	float tmp = ((b - a) / interval) * time + a;
	if (a < b)
		return tmp > b ? b : tmp;
	else
		return tmp > a ? a : tmp;

}