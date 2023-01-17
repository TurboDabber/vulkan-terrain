#pragma once

#include "app_device.hpp"
#include "app_swap_chain.hpp"
#include "app_window.hpp"

// std
#include <cassert>
#include <memory>
#include <vector>

namespace app {
    class AppRenderer {
    public:
        AppRenderer(AppWindow& window, AppDevice& device);
        ~AppRenderer();

        AppRenderer(const AppRenderer&) = delete;
        AppRenderer& operator=(const AppRenderer&) = delete;

        VkRenderPass getSwapChainRenderPass() const { return appSwapChain->getRenderPass(); }
        float getAspectRatio() const { return appSwapChain->extentAspectRatio(); }
        bool isFrameInProgress() const { return isFrameStarted; }

        VkCommandBuffer getCurrentCommandBuffer() const {
            assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
            return commandBuffers[currentFrameIndex];
        }

        int getFrameIndex() const {
            assert(isFrameStarted && "Cannot get frame index when frame not in progress");
            return currentFrameIndex;
        }

        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();

        AppWindow& appWindow;
        AppDevice& appDevice;
        std::unique_ptr<AppSwapChain> appSwapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
        int currentFrameIndex;
        bool isFrameStarted;
    };
}  // namespace app