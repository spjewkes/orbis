#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

// Include GLEW. Always include it before gl.h and glfw.h, since it's a bit magic.
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <bitset>

using namespace std;

class Window
{
public:
	Window(int width, int height, const char *title);
	virtual ~Window();

	void setTitle(const char *title);
	bool isKeyPressed(int glfwKey) const { return key_pressed[glfwKey]; }
	void swapBuffers();

private:
	static void keyboardCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

	GLFWwindow *window;
	bitset<GLFW_KEY_LAST> key_pressed;
};

#endif
