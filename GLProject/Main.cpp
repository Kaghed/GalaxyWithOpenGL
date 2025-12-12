#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include "stb_image.h"
#include "Planet.h";

using namespace glm;

int SCR_WIDTH = 1280;
int SCR_HEIGHT = 720;

Camera camera(vec3(0.0f, 15.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);


bool paused = false;
float pausedTime = 0.0f;

int main()
{
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lecture 4", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glEnable(GL_DEPTH_TEST);

	stbi_set_flip_vertically_on_load(true);

	Shader shader("./shaders/vs/L5.vs", "./shaders/fs/L5.fs");
	Shader sunShader("shaders/vs/L4.vs", "shaders/fs/L4.fs");
	Shader trailShader("shaders/vs/tr.vs", "shaders/fs/tr.fs");



	shader.use();

	mat4 projection = mat4(1.0f);
	projection = perspective(radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	//float aspectRatio = (float)SCR_WIDTH / (float)SCR_HEIGHT;
	//projection = ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, 0.1f, 100.0f);
	shader.use();
	shader.setMat4("projection", projection);


	unsigned int earthTexture;
	glGenTextures(1, &earthTexture);
	glBindTexture(GL_TEXTURE_2D, earthTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	int ewidth, eheight, enrChannels;
	unsigned char* edata = stbi_load("E:/Aghed/Hope Path/GalaxyWithOpenGL/r.jpg", &ewidth, &eheight, &enrChannels, 0);
	if (edata) {
		GLenum format = (enrChannels == 4 ? GL_RGBA : GL_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, format, ewidth, eheight, 0, format, GL_UNSIGNED_BYTE, edata);

		glGenerateMipmap(GL_TEXTURE_2D);


	}
	else {
		std::cout << "Failed to load texture: " << stbi_failure_reason() << std::endl;
	}


	stbi_image_free(edata);


	Planet earth(0.8f, 0.0f, 0.0f, 0.0f);

	unsigned int sunTexture;
	glGenTextures(1, &sunTexture);
	glBindTexture(GL_TEXTURE_2D, sunTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	int swidth, sheight, snrChannels;
	unsigned char* sdata = stbi_load("E:/Aghed/Hope Path/GalaxyWithOpenGL/ss.jpg", &swidth, &sheight, &snrChannels, 0);
	if (sdata) {
		GLenum format = (snrChannels == 4 ? GL_RGBA : GL_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, format, swidth, sheight, 0, format, GL_UNSIGNED_BYTE, sdata);

		glGenerateMipmap(GL_TEXTURE_2D);


	}
	else {
		std::cout << "Failed to load texture: " << stbi_failure_reason() << std::endl;
	}


	stbi_image_free(sdata);


	Planet sun(1.5f, 7.0f, 0.0f, 0.0f);


	unsigned int moonTexture;
	glGenTextures(1, &moonTexture);
	glBindTexture(GL_TEXTURE_2D, moonTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	int mwidth, mheight, mnrChannels;
	unsigned char* mdata = stbi_load("E:/Aghed/Hope Path/GalaxyWithOpenGL/moon.jpg", &mwidth, &mheight, &mnrChannels, 0);
	if (mdata) {
		GLenum format = (mnrChannels == 4 ? GL_RGBA : GL_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, format, mwidth, mheight, 0, format, GL_UNSIGNED_BYTE, mdata);

		glGenerateMipmap(GL_TEXTURE_2D);


	}
	else {
		std::cout << "Failed to load texture: " << stbi_failure_reason() << std::endl;
	}


	stbi_image_free(mdata);


	Planet moon(0.25f, 0.0f, 0.0f, 3.0f);
	std::vector<glm::vec3> earthTrail;

	bool hideTrail = false;
	bool timeIncreasing = false;
	bool timeStopping = false;
	bool timeReturning = false;
	bool kousouf = false;
	bool khousouf = false;
	float pauseTime = 0;
	float speedFactor = 1.0f;

	glm::vec3 sunPos(7.0f, 0.0f, 0.0f);


	unsigned int trailVAO, trailVBO;
	glGenVertexArrays(1, &trailVAO);
	glGenBuffers(1, &trailVBO);

	glBindVertexArray(trailVAO);
	glBindBuffer(GL_ARRAY_BUFFER, trailVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 11000, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glBindVertexArray(0);
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mat4 view = camera.GetViewMatrix();

		sun.model = glm::mat4(1.0f);
		sun.model = glm::translate(sun.model, glm::vec3(7.0f, 0.0f, 0.0f));

		glm::vec3 sunWorldPos = glm::vec3(sun.model[3]);

		shader.use();

		float lightRotSpeed = 0.2f;
		float time = (float)glfwGetTime();
		float angle = time * lightRotSpeed;

		glm::vec3 lightPos(
			7.0f * cos(angle),
			0.0f,
			0.0f 
		);

		shader.setVec3("lightPos", lightPos);

		shader.setVec3("lightPos", sunWorldPos);
		shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 0.5f));
		shader.setVec3("viewPos", camera.Position);
		shader.setMat4("view", view);

		 time = paused ? pausedTime : (float)glfwGetTime();
		if (kousouf || khousouf) speedFactor = 6.0f;

		float a = 15.0f, b = 11.0f;
		glm::vec3 earthPos(a * sin(time * speedFactor), 0.0f, b * cos(time * speedFactor));

		earth.model = mat4(1.0f);
		earth.model = translate(earth.model, earthPos);
		earth.model = rotate(earth.model, time * 2.0f * speedFactor, vec3(0, 0, 1));

		shader.setMat4("model", earth.model);
		glBindTexture(GL_TEXTURE_2D, earthTexture);
		shader.setInt("planetTexture", 0);
		earth.draw(shader.ID);

		sunShader.use();
		sunShader.setVec3("lightColor", glm::vec3(0.0f));
		sunShader.setMat4("projection", projection);
		sunShader.setMat4("view", view);
		sunShader.setMat4("model", sun.model);
		glBindTexture(GL_TEXTURE_2D, sunTexture);
		sunShader.setInt("sunTexture", 0);
		sun.draw(sunShader.ID);

		float moonRadius = 3.0f;
		float moonSpeed = 0.4f;
		glm::vec3 moonPos(
			earthPos.x + moonRadius * cos(time * moonSpeed * speedFactor),
			0.0f,
			earthPos.z + moonRadius * sin(time * moonSpeed * speedFactor)
		);

		moon.model = mat4(1.0f);
		moon.model = translate(moon.model, moonPos);
		moon.model = rotate(moon.model, time, vec3(0, 1, 0));

		shader.use();
		shader.setMat4("model", moon.model);
		glBindTexture(GL_TEXTURE_2D, moonTexture);
		shader.setInt("planetTexture", 0);

		if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
			hideTrail = true;
		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
			hideTrail = false;
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
			kousouf = true, hideTrail = true, earthTrail.clear();
		if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
			khousouf = true, hideTrail = true, earthTrail.clear();
		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
			kousouf = false;
			khousouf = false;
			paused = false;
			speedFactor = 1.0f;
			time = (float)glfwGetTime();
		}

		if (!hideTrail)
		{
			earthTrail.push_back(earthPos);
			if (earthTrail.size() > 11000)
				earthTrail.erase(earthTrail.begin());

			glBindBuffer(GL_ARRAY_BUFFER, trailVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, earthTrail.size() * sizeof(glm::vec3), earthTrail.data());

			trailShader.use();
			trailShader.setMat4("view", view);
			trailShader.setMat4("projection", projection);

			glBindVertexArray(trailVAO);
			glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(earthTrail.size()));
			glBindVertexArray(0);
		}

		moon.draw(shader.ID);

		if ((kousouf || khousouf) && time != pauseTime && earthPos.x > 14.0f)
		{
			if (kousouf) {
				glm::vec3 sn(sunPos);
				sn.z -= 0.8f;
				glm::vec3 v = earthPos - sn;
				glm::vec3 w = moonPos - sn;

				float t = glm::dot(w, v) / glm::dot(v, v);
				float d = glm::length(glm::cross(w, v)) / glm::length(v);

				if (t > 0.2f && t < 1.3f && d < 0.2f) {
					paused = true;
					pausedTime = time;
				}
			}

			if (khousouf) {
				glm::vec3 sn(sunPos);

				glm::vec3 v = earthPos - sn;
				glm::vec3 w = moonPos - sn;

				float t = glm::dot(w, v) / glm::dot(v, v);
				float d = glm::length(glm::cross(w, v)) / glm::length(v);

				float earthRadius = 0.5f;
				float shadowRadius = earthRadius * 2.0f;
		
				if (t > 1.0f && d < shadowRadius && earthPos.x > 14.0f && earthPos.x < 14.2f) {
					paused = true;
					pausedTime = time;

				
				}
			}
		}

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

	static bool pPressedLastFrame = false;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		if (!pPressedLastFrame) {
			paused = !paused;
			if (paused) {
				pausedTime = (float)glfwGetTime();
			}
		}
		pPressedLastFrame = true;
	}
	else {
		pPressedLastFrame = false;
	}
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
