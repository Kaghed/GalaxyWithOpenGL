#include "Planet.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

Planet::Planet(float rad, float x, float y, float z) {
    this->rad = rad;
    center = glm::vec3(x, y, z);

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    int horiz = 36;
    int verti = 18;
    float PI = glm::pi<float>();

    for (int i = 0; i <= verti; i++) {
        float phi = PI / 2 - i * PI / verti;

        for (int j = 0; j <= horiz; j++) {
            float theta = j * 2 * PI / horiz;

            float px = rad * cos(phi) * cos(theta);
            float py = rad * cos(phi) * sin(theta);
            float pz = rad * sin(phi);

            // normals (normalized)
            float nx = px / rad;
            float ny = py / rad;
            float nz = pz / rad;

            float u = (float)j / horiz;
            float v = 1.0f - (float)i / verti;

            vertices.push_back(px);
            vertices.push_back(py);
            vertices.push_back(pz);

            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);

            vertices.push_back(u);
            vertices.push_back(v);
        }
    }

    for (int i = 0; i < verti; i++) {
        for (int j = 0; j < horiz; j++) {
            int first = i * (horiz + 1) + j;
            int second = first + horiz + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    indicesCount = indices.size();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

  
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    model = glm::mat4(1.0f);
    model = glm::translate(model, center);
}

void Planet::draw(unsigned int shaderID) {
    glUseProgram(shaderID);

    int modelLoc = glGetUniformLocation(shaderID, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Planet::update(glm::vec3 pos, float selfAngle, float tiltAngle) {
    model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::rotate(model, selfAngle, glm::vec3(0, 1, 0));
    model = glm::rotate(model, tiltAngle, glm::vec3(1, 0, 0));
}
