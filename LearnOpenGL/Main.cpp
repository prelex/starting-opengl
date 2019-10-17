#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include <stb-master/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"

void processInput(GLFWwindow* window);

// Callback functions. Called whenever the user changes the window size, uses the mouse, or uses the scroll wheel
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// check if camera (user) is standing on the switch
bool switchPressed();

unsigned int &loadTexture( const char* name);

const unsigned int screenWidth = 800, screenHeight = 600;

// Uniforms used in the fragment shaders. Defines the mixing between two textures
float mixValue = 0.2f, switchMixValue = 0.0f;

// Used to make camera speed independent of framerate
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Used to smooth initial mouse input
float lastX = screenWidth / 2.0f, lastY = screenHeight / 2.0f;
bool firstMouse = true;

// set up the camera
Camera camera(glm::vec3(0.0f, 0.0f, 20.0f));

glm::vec3 switchPosition = glm::vec3(-0.15f, -1.5f, 15.0f);

int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw: create window
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// create and load textures
	unsigned int cppLogo1 = loadTexture("cppLogo1.png");
	unsigned int cppLogo2 = loadTexture("cppLogo2.png"); 
	unsigned int redSwitchTexture = loadTexture("red_power_button.png");
	unsigned int greenSwitchTexture = loadTexture("green_power_button.png");

	// load appropriate vertex and fragment shaders, and create shader programs
	Shader cubeShader("cubeVertexShader.txt", "cubeFragmentShader.txt");
	Shader switchShader("switchVertexShader.txt", "switchFragmentShader.txt");

	glViewport(0, 0, 800, 600);

	// cube vertices
	float vertices[] = {
	//   xyz coords       // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	// same as above, just without texture coords
	float cubeVertices[] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
	}; 

	// positions of the 15 cubes to spell out "C++"
	glm::vec3 cubePositions[] = {
	 glm::vec3(-6.0f,  0.0f,  0.0f),   // C
	 glm::vec3(-5.5f,  1.25f, 0.0f),   // 
	 glm::vec3(-5.5f, -1.25f,  0.0f),  //  
	 glm::vec3(-4.0f, 1.5f,  0.0f),    // 
	 glm::vec3(-4.0f, -1.5f,  0.0f),   // _______
	 glm::vec3(-2.0f,  0.0f,  0.0f),   // +
	 glm::vec3(-0.5f, 0.0f,  0.0f),    // 
	 glm::vec3(1.0f,  0.0f,  0.0f),    //
	 glm::vec3(-0.5f,  1.5f,  0.0f),   //
	 glm::vec3(-0.5f,  -1.5f,  0.0f),  // _______
	 glm::vec3(3.0f,  0.0f,  0.0f),    // +
	 glm::vec3(4.5f, 0.0f,  0.0f),	   // 
	 glm::vec3(6.0f,  0.0f,  0.0f),	   // 
	 glm::vec3(4.5f,  1.5f,  0.0f),	   //
	 glm::vec3(4.5f,  -1.5f,  0.0f)	   // 
	};								 

	// switch vertices
	float switchVertices[] = {
		// position				// texture
		-0.5f,  0.5f, 0.0f,		0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,		1.0f, 1.0f,
		 0.5f,  -0.5f, 0.0f,	1.0f, 0.0f,
		 0.5f,  -0.5f, 0.0f,	1.0f, 0.0f,
		-0.5f,  -0.5f, 0.0f,	0.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,		0.0f, 1.0f
	};

	// create vertex buffers and vertex arrays
	unsigned int VBOs[3], VAOs[3]; 
	glGenBuffers(3, VBOs);
	glGenVertexArrays(3, VAOs);

	//------------------------------------------------------------
	//------------------------------------------------------------
	// set up cube vertex array
	glBindVertexArray(VAOs[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texture attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	//------------------------------------------------------------
	//------------------------------------------------------------


	//------------------------------------------------------------
	//------------------------------------------------------------
	// set up switch vertex array
	glBindVertexArray(VAOs[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(switchVertices), switchVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	//------------------------------------------------------------
	//------------------------------------------------------------

	//------------------------------------------------------------
	//------------------------------------------------------------
	// set up light source (lamp) vertex array
	glBindVertexArray(VAOs[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	//------------------------------------------------------------
	//------------------------------------------------------------


	glEnable(GL_DEPTH_TEST);

	// used when the user is on the switch
	float angle = 0.0f, lastAngle = (float)glfwGetTime();

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		// clear the color buffer and depth buffer
		glClearColor(0.0f, 0.18f, 0.46f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cppLogo1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, cppLogo2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, redSwitchTexture);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, greenSwitchTexture);

		// activate the cube shader program
		cubeShader.use();

		// tell OpenGL which shader sampler belongs to which texture unit
		cubeShader.setInt("cppLogo1", 0);
		cubeShader.setInt("cppLogo2", 1);

		// set the cube fragment shader's mix value to oscillate between 0 and 1
		mixValue = (float)sin(4*currentFrame) / 2.0f + 0.5f;
		cubeShader.setFloat("mixValue", mixValue);

		// set up the camera
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		cubeShader.setMat4("projection", projection);
		glm::mat4 view = camera.getViewMatrix();
		cubeShader.setMat4("view", view);

		// draw the cubes
		glBindVertexArray(VAOs[0]);
		for (unsigned int i = 0; i < 15; ++i)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			
			if (switchPressed())
			{
				angle = lastAngle + 4 * cos(4 * currentFrame) * deltaTime; // f(t + dt) = f(t) + (df/dt) dt, where f(t) = sin(4t);
				model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			else
				model = glm::rotate(model, lastAngle, glm::vec3(0.0f, 1.0f, 0.0f));
			cubeShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		lastAngle = angle;

		// switch shader
		switchShader.use();
		switchShader.setInt("redSwitchTexture", 2);
		switchShader.setInt("greenSwitchTexture", 3);
		if (switchPressed())
			switchShader.setFloat("switchMixValue", 1);
		else
			switchShader.setFloat("switchMixValue", 0);
		switchShader.setMat4("projection", projection);
		switchShader.setMat4("view", view);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, switchPosition);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		switchShader.setMat4("model", model);

		// draw the switch
		glBindVertexArray(VAOs[1]);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	float cameraSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos;
	lastX = (float)xpos;
	lastY = (float)ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll((float)yoffset);
}

bool switchPressed()
{
	glm::vec3 temp(camera.Position.x, switchPosition.y, camera.Position.z);
	return glm::distance(temp, switchPosition) <= 0.5f;
}

unsigned int &loadTexture( const char* name)
{
	unsigned int id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	// set the texture filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load and generate texture
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(name, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	return id;
}