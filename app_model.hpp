#pragma once

#include "app_device.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>
#include <memory>
#include <vector>
namespace app
{
	class AppModel {
	public:
		struct Vertex
		{
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};
			static std::vector<VkVertexInputBindingDescription> getBindingDescription();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescription();
			bool operator==(const Vertex& other) const {
				return position == other.position && color == other.color && normal == other.normal &&
					uv == other.uv;
			}
		};
		//AppModel(AppDevice& device, const std::vector<Vertex>& vertices);
		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string& filepath);
			bool loadOBJ(
				const char* path,
				std::vector<glm::vec3>& out_vertices,
				std::vector<glm::vec2>& out_uvs,
				std::vector<glm::vec3>& out_normals
			);
		};

		AppModel(AppDevice& device, const AppModel::Builder& builder);
		~AppModel();
		AppModel(const AppModel&) = delete;
		AppModel& operator=(const AppModel&) = delete;

		static std::unique_ptr<AppModel> createModelFromFile(
			AppDevice& device, const std::string& filepath);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);
		AppDevice& appDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		uint32_t indexCount;
	};

}