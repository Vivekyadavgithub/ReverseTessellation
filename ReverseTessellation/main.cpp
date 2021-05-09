#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main()
{
	GLFWwindow* window;
	if (!glfwInit()) return -1;

	window = glfwCreateWindow(1980, 1080, "Engine", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW ERROR\n";
	}
}