#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <iostream>
#include <vector>

#include "shader.h"

struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh {
public:
	/*  Mesh Data  */
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	unsigned int _vao;

	/*  Functions  */
	// constructor
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	// render the mesh
	void draw(Shader shader);

private:
	/*  Render data  */
	unsigned int _vbo, _ebo;

	/*  Functions    */
	// initializes all the buffer objects/arrays
	void setupMesh();
};