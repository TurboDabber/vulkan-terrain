#pragma once

#include "app_device.hpp"
#include "app_game_object.hpp"
#include "app_pipeline.hpp"
#include "app_camera.hpp"
// std
#include <memory>
#include <vector>

namespace app {
	class SimpleRenderSystem {
	public:
		SimpleRenderSystem(AppDevice& device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects(
			VkCommandBuffer commandBuffer,
			std::vector<AppGameObject>& gameObjects,
			const AppCamera& camera);
		void setWireframe(bool set, VkRenderPass renderPass);
	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		AppDevice& appDevice;

		std::unique_ptr<AppPipeline> appPipeline;
		VkPipelineLayout pipelineLayout;
	};
}