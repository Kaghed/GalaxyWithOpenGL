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

	Shader ourShader("./shaders/vs/L4.vs", "./shaders/fs/L4.fs");
//	Shader ourShader("./shaders/vs/L5.vs", "./shaders/fs/L5.fs");
	Shader allShaders("./shaders/vs/L4.vs", "./shaders/fs/L4.fs");



	ourShader.use();
	
	mat4 projection = mat4(1.0f);
	projection = perspective(radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	//float aspectRatio = (float)SCR_WIDTH / (float)SCR_HEIGHT;
	//projection = ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, 0.1f, 100.0f);
	ourShader.setMat4("projection", projection);

	unsigned int earthTexture;
	glGenTextures(1, &earthTexture);
	glBindTexture(GL_TEXTURE_2D, earthTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	int ewidth, eheight, enrChannels;
	unsigned char* edata = stbi_load("E:/Aghed/Hope Path/Galaxy/r.jpg", &ewidth, &eheight, &enrChannels, 0);
	if (edata) {
		GLenum format = (enrChannels == 4 ? GL_RGBA : GL_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, format,ewidth, eheight, 0, format, GL_UNSIGNED_BYTE, edata);

		glGenerateMipmap(GL_TEXTURE_2D);

	
	} else {
		std::cout << "Failed to load texture: " << stbi_failure_reason() << std::endl;
	}


	stbi_image_free(edata);


	Planet earth(0.5f, 0.0f, 0.0f, 0.0f);        

	unsigned int sunTexture;
	glGenTextures(1, &sunTexture);
	glBindTexture(GL_TEXTURE_2D, sunTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	int swidth, sheight, snrChannels;
	unsigned char* sdata = stbi_load("E:/Aghed/Hope Path/Galaxy/ss.jpg", &swidth, &sheight, &snrChannels, 0);
	if (sdata) {
		GLenum format = (snrChannels == 4 ? GL_RGBA : GL_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, format,swidth, sheight, 0, format, GL_UNSIGNED_BYTE, sdata);

		glGenerateMipmap(GL_TEXTURE_2D);


	}
	else {
		std::cout << "Failed to load texture: " << stbi_failure_reason() << std::endl;
	}


	stbi_image_free(sdata);


	Planet sun(1.5f, 7.0f, -1.0f, 0.0f);
	
		
	unsigned int moonTexture;
	glGenTextures(1, &moonTexture);
	glBindTexture(GL_TEXTURE_2D, moonTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	int mwidth, mheight, mnrChannels;
	unsigned char* mdata = stbi_load("E:/Aghed/Hope Path/Galaxy/moon.jpg", &mwidth, &mheight, &mnrChannels, 0);
	if (mdata) {
		GLenum format = (mnrChannels == 4 ? GL_RGBA : GL_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, format, mwidth, mheight, 0, format, GL_UNSIGNED_BYTE, mdata);

		glGenerateMipmap(GL_TEXTURE_2D);


	}
	else {
		std::cout << "Failed to load texture: " << stbi_failure_reason() << std::endl;
	}


	stbi_image_free(mdata);


	Planet moon(0.25f, 0.0f, -0.5f, 3.0f);
	std::vector<glm::vec3> earthTrail;

	bool hideTrail = false;
	bool timeIncreasing = false;
	bool timeStopping = false;
	bool timeReturning = false;
	bool kousouf = false;
	bool khousouf = false;
	float pauseTime = 0;
	float speedFactor = 1.0f; 

	glm::vec3 sunPos(0.0f, -1.0f, 0.0f);



	while (!glfwWindowShouldClose(window))
	{
		{
			float currentFrame = static_cast<float>(glfwGetTime());
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			processInput(window);

			glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			mat4 view = mat4(1.0f);
			view = view = camera.GetViewMatrix();
			ourShader.setMat4("view", view);

			float time = (float)glfwGetTime();

			
		}

		

		ourShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, earthTexture);
		glUniform1i(glGetUniformLocation(ourShader.ID, "planetTexture"), 0);
		float time = (float)glfwGetTime();

		if (paused) {
			time = pausedTime;
		}
		if (kousouf || khousouf)
			speedFactor = 6.0f; 

		float a = 15.0f, b = 11.0f;

		glm::vec3 earthPos(a* sin(time * speedFactor), 0.0f, b* cos(time * speedFactor));
		earth.model = glm::mat4(1.0f);
		earth.model = glm::translate(earth.model, earthPos);

		earth.model = glm::rotate(earth.model, time * speedFactor, glm::vec3(0.0f, 1.0f, 0.0f));

		earth.model = glm::rotate(earth.model, time / 2 * speedFactor, glm::vec3(1.0f, 0.0f, 0.0f));



	

		earth.draw(ourShader.ID);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sunTexture);
		glUniform1i(glGetUniformLocation(ourShader.ID, "planetTexture"), 0);

		sun.draw(ourShader.ID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, moonTexture);
		glUniform1i(glGetUniformLocation(ourShader.ID, "planetTexture"), 0);



		float moonRadius = 3.0f;     
		float moonSpeed = 0.4f;       

		glm::vec3 moonPos(
			earthPos.x + moonRadius * cos(time * moonSpeed * speedFactor),
			-0.5f,
			earthPos.z + moonRadius * sin(time * moonSpeed * speedFactor)
		);
		moon.model = glm::mat4(1.0f);
		moon.model = glm::translate(moon.model, moonPos);

		moon.model = glm::mat4(1.0f);
		moon.model = glm::translate(moon.model, moonPos);

		moon.model = glm::rotate(moon.model, time, glm::vec3(0.0f, 1.0f, 0.0f));
	
		
		glm::vec3 higherEarthPos = earthPos;

		
		higherEarthPos.x -= 7.0f;
		if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) 
			hideTrail = true;
		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
			hideTrail = false; 
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
			kousouf = true, hideTrail = true, earthTrail.clear();
		if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
			khousouf = true ,hideTrail = true, earthTrail.clear();
		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS){
			kousouf = false , paused = false;
			khousouf = false;
			speedFactor = 1.0f;
			time = (float)glfwGetTime();
		}
	
		glColor3f(1.0f, 1.0f, 1.0f);


		glBegin(GL_LINE_STRIP);
		glVertex3f(0.0f, -1.0f, 0.0f);
		glVertex3f(earthPos.x - 7, earthPos.y, earthPos.z);
		glEnd();

		if(!hideTrail)
		{

			earthTrail.push_back(higherEarthPos);

			if (earthTrail.size() > 11000) {
				earthTrail.erase(earthTrail.begin());
			}


			glBegin(GL_LINE_STRIP);
			for (auto& p : earthTrail) {
				glVertex3f(p.x, p.y+1.0f, p.z);
			}
		

			glEnd();
		}

		if ((kousouf || khousouf) && time!=pauseTime &&earthPos.x > 14.0f)
		{
			timeIncreasing = true;
			
		///// add kousouf things here 
			
			cout << earthPos.x << " " << moonPos.x << " ";

			if(kousouf){

				glm::vec3 sunPos(0.0f, -1.0f, 0.0f);
				glm::vec3 v = earthPos - sunPos;
				glm::vec3 w = moonPos - sunPos;


				float t = glm::dot(w, v) / glm::dot(v, v);


				float d = glm::length(glm::cross(w, v)) / glm::length(v);

			if (t > 0.0f && t < 1.0f && d < 0.3f) {
				paused = true;
				pausedTime = time;
			}
			}
			if (khousouf) {
    glm::vec3 v = earthPos - sunPos; 
    glm::vec3 w = moonPos - sunPos;  

    float t = glm::dot(w, v) / glm::dot(v, v); 
    float d = glm::length(glm::cross(w, v)) / glm::length(v); 

    float earthRadius = 0.5f; 
    float distSunEarth = glm::length(v);
    float distSunMoon  = glm::length(w);

  
    float shadowRadius = earthRadius * 2.0f;

    if (t > 1.0f && d < shadowRadius && earthPos.x > 14.0f) {
        paused = true;
        pausedTime = time;
    
	}
}




		}

		moon.draw(ourShader.ID);
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
