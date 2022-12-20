#pragma once
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
using namespace std;
//��������ת������
class BezierFace
{
	//������
	int numVertices;
	//������(ÿ���ı���Ƭ6������)
	int numIndices;

	float texrange_l, texrange_r;

	//uv �ֶ���

	//attributes
	vector<glm::vec3> vertices;
	vector<glm::vec2> texCoords;
	vector<glm::vec3> normals;
	vector<float> us;
	vector<float> rs;
	vector<float> ys;

	vector<int> indices;
	//����
	void generate(int prec);
	//���Ƶ�
	float* controlPoints;
	vector<glm::vec2> controlPointsVector;

	float toRadians(float degrees);
	float Bernstein(float u, int index);
public:

	BezierFace();
	//BezierFace(int i); 
	BezierFace(vector<glm::vec2> controlPointsVector, float l = 0, float r = 1);

	float getLength();
	int getNumVertices();
	int getNumIndices();
	vector<glm::vec3> getVertices();
	vector<glm::vec3> getNormals();
	vector<glm::vec2> getTexCoords();
	vector<int> getIndices();

	bool getRadiance(float pos, float& y, float& r);


};
