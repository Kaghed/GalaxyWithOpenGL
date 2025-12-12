#pragma once
#include <glm/glm.hpp>

class Planet {
public:
    unsigned int VAO, VBO, EBO;
    unsigned int indicesCount;
    unsigned int vertexCount;
    float rad;
    glm::vec3 center;
    glm::mat4 model;

    Planet(float rad, float x, float y, float z);
    void draw(unsigned int shaderID);
    void update(glm::vec3 pos, float selfAngle, float tiltAngle);
};
