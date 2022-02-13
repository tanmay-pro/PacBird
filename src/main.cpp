#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLFW_INCLUDE_GLCOREARB
#include "game.h"
#include "resource_manager.h"

#include <iostream>

// GLFW function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// The Width of the screen
const unsigned int SCREEN_WIDTH = 1800;
// The height of the screen
const unsigned int SCREEN_HEIGHT = 1000;

Game Breakout(SCREEN_WIDTH, SCREEN_HEIGHT);

//
//glm::mat4 trans = glm::mat4(1.0f);

int main(int argc, char *argv[])
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_RESIZABLE, false);
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	// OpenGL configuration
	// --------------------
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// initialize game
	// ---------------
	Breakout.Init();
	
	// deltaTime variables
	// -------------------
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	
	while (!glfwWindowShouldClose(window))
	{
		// calculate delta time
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents();
		
		// manage user input
		// -----------------
		Breakout.ProcessInput(deltaTime);
		
		// update game state
		// -----------------
		Breakout.Update(deltaTime);
		
		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		Breakout.Render();
		
		glfwSwapBuffers(window);
	}
	
	// delete all resources as loaded using the resource manager
	// ---------------------------------------------------------
	ResourceManager::Clear();
	
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// when a user presses the escape key, we set the WindowShouldClose property to true, closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			Breakout.Keys[key] = true;
		else if (action == GLFW_RELEASE)
			Breakout.Keys[key] = false;
	}
	if(key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if(!Breakout.light)
			Breakout.light = true;
		else
			Breakout.light = false;
	}
//	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//	{
//		trans = glm::translate(trans, glm::vec3(-0.05f, 0.0f, 0.0f));
//	}
//	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//	{
//		trans = glm::translate(trans, glm::vec3(+0.05f, 0.0f, 0.0f));
//	}
//	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//	{
//		trans = glm::translate(trans, glm::vec3(0.0f, -0.05f, 0.0f));
//	}
//	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//	{
//		trans = glm::translate(trans, glm::vec3(0.0f, 0.05f, 0.0f));
//	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}