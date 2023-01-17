#pragma once

#include "app_device.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>

#include <vector>
namespace app
{
	class AppModel {
	public:
		struct Vertex
		{
			glm::vec3 position{};
			glm::vec3 color{};
			static std::vector<VkVertexInputBindingDescription> getBindingDescription();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescription();
		};
		AppModel(AppDevice& device, const std::vector<Vertex>& vertices);;
		~AppModel();
		AppModel(const AppModel&) = delete;
		AppModel& operator=(const AppModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		AppDevice& appDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
	};

}