#pragma once
#include <glm/glm.hpp>
class Planet
{
public:
	Planet(float rad, float x, float y, float z);
	//~Planet();
	void update(glm::vec3 pos, float selfAngle, float tiltAngle);
	void draw(unsigned int shaderID);
	//glm::mat4 getModelMatrix() const; 
	void rotateY(float angle);
	void rotateX(float angle);
	//void setColor(glm::vec3 newColor); 
	//void setRadius(float newRadius);   
	void rotateSelf(float angle);
	void rotate(float angleY , float angleX);
	glm::mat4 model;

private:
	float rad;
	glm::vec3 center;
	unsigned int VAO, VBO ,EBO;
	int vertexCount;
	int indicesCount;
};

