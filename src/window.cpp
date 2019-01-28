#include <cstdlib>
#include <iostream>
#include "window.hpp"

Window::Window(int width, int height, const char *title) : width(width), height(height),
														   xposoffset(static_cast<double>(width) / 2.0),
														   yposoffset(static_cast<double>(height) / 2.0)
{
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Set up for OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

	window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL)
	{
		cerr << "Failed to open GLFW window.\n";
		abort();
	}

	glfwSetWindowUserPointer(window, this);

	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, Window::keyboardCallback);
	glfwSetCursorPosCallback(window, Window::mouseposCallback);

	// Removes vsync limit
	glfwSwapInterval(0);
}

Window::~Window()
{
	glfwDestroyWindow(window);
}

void Window::setTitle(const char *title)
{
	glfwSetWindowTitle(window, title);
}

void Window::getMousePos(double &xpos, double &ypos)
{
	xpos = current_xpos;
	ypos = current_ypos;
}

void Window::swapBuffers()
{
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Window::keyboardCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	Window *this_ptr = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (action == GLFW_PRESS)
	{
		this_ptr->key_pressed.set(key);
	}
	else if (action == GLFW_RELEASE)
	{
		this_ptr->key_pressed.reset(key);
	}
}

void Window::mouseposCallback(GLFWwindow *window, double xpos, double ypos)
{
	Window *this_ptr = static_cast<Window*>(glfwGetWindowUserPointer(window));

	this_ptr->current_xpos = xpos - this_ptr->xposoffset;
	this_ptr->current_ypos = ypos - this_ptr->yposoffset;
}
