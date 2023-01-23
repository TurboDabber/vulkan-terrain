#pragma once
#include "app_device.hpp"
#include "app_renderer.hpp"
#include "app_window.hpp"
#include "app_game_object.hpp"
#include <memory>
#include <vector>
#include <array>
#include "perlin_generator.hpp"
namespace app {
	class DefaultApp
	{
	public:
		static constexpr int _WIDTH_ = 1200;
		static constexpr int _HEIGHT_ = 900;
		static constexpr int _CHUNK_SIZE_ = 40;
		PerlinGenerator perlin;
		DefaultApp();
		~DefaultApp();
		
		DefaultApp(const DefaultApp&) = delete;
		DefaultApp& operator=(const DefaultApp&) = delete;

		void run();
	private:
		void loadGameObjects();
		void createPipelineLayout();
		void createPipeLine();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);
		void renderGameObjects(VkCommandBuffer commandBuffer);
		std::unique_ptr<AppModel> createCubeModel(AppDevice& device, glm::vec3 offset);
		std::unique_ptr<AppModel> createChunk(AppDevice& device, glm::vec3 offset, int chunkX, int chunkZ);
		AppWindow appWindow{ _WIDTH_, _HEIGHT_, "Interactive and generated terrain in vulkan API" };
		AppDevice appDevice{ appWindow };
		AppRenderer appRenderer{ appWindow, appDevice };
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<AppGameObject> gameObjects;
	};
}