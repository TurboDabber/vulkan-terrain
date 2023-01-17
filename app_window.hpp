#pragma once
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULCAN

#include<GLFW/glfw3.h>

#include <string>
namespace app
{

	class AppWindow {
	public:
		AppWindow(int w, int h, std::string name);
		~AppWindow();
		//prevents double window
		AppWindow(const AppWindow &) = delete;
		AppWindow& operator=(const AppWindow&) = delete;
		bool isClosing()
		{
			return glfwWindowShouldClose(window);
		}
		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
		bool wasWindowResized()
		{
			return isFrameBufferResized;
		}
		void resetWindowResizedFlag()
		{
			isFrameBufferResized = false;
		}
		GLFWwindow* getGLFWwindow() const { return window; }
		void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
	private:
		static void frameBufferResizeCallback(GLFWwindow* window, int w, int h);
		void initWindow();

		int width;
		int height;
		bool isFrameBufferResized = false;
		std::string windowName;
		GLFWwindow* window;
	};
}