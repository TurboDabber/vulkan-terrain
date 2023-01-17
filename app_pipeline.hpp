#pragma once
#include "app_device.hpp"

#include <string>
#include <vector>
namespace app
{
	struct PipelineConfigInfo{
		  PipelineConfigInfo(const PipelineConfigInfo&) = default;
		  PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		  VkPipelineViewportStateCreateInfo viewportInfo;
		  VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		  VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		  VkPipelineMultisampleStateCreateInfo multisampleInfo;
		  VkPipelineColorBlendAttachmentState colorBlendAttachment;
		  VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		  VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		  std::vector<VkDynamicState> dynamicStateEnables;
		  VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		  VkPipelineLayout pipelineLayout = nullptr;
		  VkRenderPass renderPass = nullptr;
		  uint32_t subpass = 0;
	};

	class AppPipeline
	{
		public:
			AppPipeline(AppDevice &device, const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo info);
			~AppPipeline();
			AppPipeline(const AppPipeline&) = delete;
			void operator=(const AppPipeline&) = delete;
			void bind(VkCommandBuffer commandBuffer);
			static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo, bool isWireframe);
		private:
			static std::vector<char> readFile(const std::string& filePath);
			void createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& info);
			void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		
			AppDevice& appDevice;
			VkPipeline graphicsPipeline;
			VkShaderModule vertShaderModule;
			VkShaderModule fragShaderModule;
	};
}