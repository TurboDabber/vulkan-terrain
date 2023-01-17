#include "app_renderer.hpp"

// std
#include <array>
#include <cassert>
#include <stdexcept>

namespace app {

    AppRenderer::AppRenderer(AppWindow& window, AppDevice& device)
        : appWindow{ window }, appDevice{ device } {
        recreateSwapChain();
        createCommandBuffers();
    }

    AppRenderer::~AppRenderer() { freeCommandBuffers(); }

    void AppRenderer::recreateSwapChain() {
        auto extent = appWindow.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = appWindow.getExtent();
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(appDevice.device());

        if (appSwapChain == nullptr) {
            appSwapChain = std::make_unique<AppSwapChain>(appDevice, extent);
        }
        else {
            std::shared_ptr<AppSwapChain> oldSwapChain = std::move(appSwapChain);
            appSwapChain = std::make_unique<AppSwapChain>(appDevice, extent, oldSwapChain);

            if (!oldSwapChain->compareSwapFormats(*appSwapChain.get())) {
                throw std::runtime_error("Swap chain image(or depth) format has changed!");
            }
        }
    }

    void AppRenderer::createCommandBuffers() {
        commandBuffers.resize(AppSwapChain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = appDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(appDevice.device(), &allocInfo, commandBuffers.data()) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void AppRenderer::freeCommandBuffers() {
        vkFreeCommandBuffers(
            appDevice.device(),
            appDevice.getCommandPool(),
            static_cast<uint32_t>(commandBuffers.size()),
            commandBuffers.data());
        commandBuffers.clear();
    }

    VkCommandBuffer AppRenderer::beginFrame() {
        assert(!isFrameStarted && "Can't call beginFrame while already in progress");

        auto result = appSwapChain->acquireNextImage(&currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return nullptr;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        isFrameStarted = true;

        auto commandBuffer = getCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }
        return commandBuffer;
    }

    void AppRenderer::endFrame() {
        assert(isFrameStarted && "Can't call endFrame while frame is not in progress");
        auto commandBuffer = getCurrentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }

        auto result = appSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
            appWindow.wasWindowResized()) {
            appWindow.resetWindowResizedFlag();
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % AppSwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void AppRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
        assert(
            commandBuffer == getCurrentCommandBuffer() &&
            "Can't begin render pass on command buffer from a different frame");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = appSwapChain->getRenderPass();
        renderPassInfo.framebuffer = appSwapChain->getFrameBuffer(currentImageIndex);

        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = appSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(appSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(appSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{ {0, 0}, appSwapChain->getSwapChainExtent() };
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void AppRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
        assert(
            commandBuffer == getCurrentCommandBuffer() &&
            "Can't end render pass on command buffer from a different frame");
        vkCmdEndRenderPass(commandBuffer);
    }

}