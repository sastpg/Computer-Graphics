#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

class SkyBox {
public:
	SkyBox(int mode);

	~SkyBox();

	void draw();

private:
    GLuint _vao = 0;
    GLuint _vbo = 0;
    unsigned int cubemapTexture = 0;

    // load skybox, id: cubemapTexture
    // refer to https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/06%20Cubemaps/
    unsigned int loadCubemap(std::vector<std::string> faces);
    GLfloat _vertices[108] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    std::vector<std::string> faces1
    {
        "media/texture/skybox1/Right_Tex.jpg",  //r
        "media/texture/skybox1/Left_Tex.jpg",  //l
        "media/texture/skybox1/Up_Tex.jpg",  //u
        "media/texture/skybox1/Down_Tex.jpg",  //d
        "media/texture/skybox1/Front_Tex.jpg",  //f
        "media/texture/skybox1/Back_Tex.jpg"  //b
    };

    std::vector<std::string> faces2
    {
        "media/texture/skybox2/front.png",  //r
        "media/texture/skybox2/front.png",  //l
        "media/texture/skybox2/top.png",  //u
        "media/texture/skybox2/metal.png",  //d
        "media/texture/skybox2/front.png",  //f
        "media/texture/skybox2/front.png",  //b
    };
};
