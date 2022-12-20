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

#define SCR_WIDTH 1920
#define SCR_HEIGHT 1080

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

DIYmodel diymodel;

void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


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
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
	Model vase("media/obj/vase/HSM0037.obj");
    Shader shaderBall("media/shader/pbr.vs", "media/shader/pbr.fs");
    Shader skyboxShader1("media/shader/skybox2.vs", "media/shader/skybox2.fs");
    SkyBox skybox2(2);
    Shader floorShader("media/shader/blending.vs", "media/shader/blending.fs");
    Shader lightingShader("media/shader/light_casters.vs", "media/shader/light_casters.fs");
    Shader reconShader("media/shader/recon.vs", "media/shader/recon.fs");
    Shader lightingShader1("media/shader/light_casters1.vs", "media/shader/light_casters1.fs");
    //PlyModel happy_recon1_0(name4, 0);
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
	diymodel.remake();
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
			if (light == 0)
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
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.8f)); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(-90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
			model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));	// it's a bit too big for our scene, so scale it down
			lightingShader.setMat4("model", model);
			vase.draw(lightingShader);

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