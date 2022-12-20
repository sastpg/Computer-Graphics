#pragma once
#include <glm/glm.hpp>

unsigned int loadTexture(char const* path);
unsigned char* loadData(char const* path);
unsigned int loadTextureD(unsigned char* data, int width, int height, int nrComponents);
void render(unsigned int& quadVAO, unsigned int& quadVBO, glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec3 pos4, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, glm::vec2 uv4);

void renderQuad();
void renderQuad1();
void renderQuad2();
void renderSphere();
