#include "app_window.hpp"
#include <stdexcept>

namespace app {
	AppWindow::AppWindow(int w, int h, std::string n) : width{ w }, height{ h }, windowName{ n }
	{
		initWindow();
	}

	AppWindow::~AppWindow()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void AppWindow::initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
	}

	void AppWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("Window creation \"createWindowSurface()\" failed");
		}
	}

	void AppWindow::frameBufferResizeCallback(GLFWwindow* window, int w, int h)
	{
		auto appWindow = reinterpret_cast<AppWindow*>(glfwGetWindowUserPointer(window));
		appWindow->isFrameBufferResized = true;
		appWindow->width = w;
		appWindow->height = h;
	}
}