#include "app_model.hpp"

#include <vector>
#include <cassert>
namespace app
{

	AppModel::AppModel(AppDevice& device, const std::vector<Vertex>& vertices) :appDevice{device}
	{
		createVertexBuffers(vertices);
	}

	AppModel::~AppModel()
	{
		vkDestroyBuffer(appDevice.device(), vertexBuffer, nullptr);
		vkFreeMemory(appDevice.device(), vertexBufferMemory, nullptr);
	}

	void AppModel::createVertexBuffers(const std::vector<Vertex>& vertices) 
	{
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		appDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vertexBuffer,
			vertexBufferMemory);
		void* data;
		vkMapMemory(appDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(appDevice.device(), vertexBufferMemory);
	}
	void AppModel::draw(VkCommandBuffer commandBuffer)
	{
		vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
	}
	void AppModel::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
	}

	std::vector<VkVertexInputBindingDescription> AppModel::Vertex::getBindingDescription()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> AppModel::Vertex::getAttributeDescription()
	{
		std::vector< VkVertexInputAttributeDescription> attributeDescription(2);
		attributeDescription[0].binding = 0;
		attributeDescription[0].location = 0;
		attributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescription[0].offset = offsetof(Vertex, position);

		attributeDescription[1].binding = 0;
		attributeDescription[1].location = 1;
		attributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescription[1].offset = offsetof(Vertex, color);
		return attributeDescription;
	}

}