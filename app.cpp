#include "app.hpp"
#include "simple_render_system.hpp"
#include "app_camera.hpp"
#include "keyboard_movement_controller.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/constants.hpp>
#include <cassert>
#include <chrono>
#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace app {

	DefaultApp::DefaultApp() { loadGameObjects(); }

	DefaultApp::~DefaultApp() {}

	void DefaultApp::run() {
        perlin=PerlinGenerator();
		SimpleRenderSystem simpleRenderSystem{ appDevice, appRenderer.getSwapChainRenderPass() };
        AppCamera camera{};
        std::cout << "MaxPushConstantSize: " << appDevice.properties.limits.maxPushConstantsSize << "\n";
        auto viewerObject = AppGameObject::createGameObject();
        KeyboardMovementController cameraController{};
        bool isWireframe = false;
        auto currentTime = std::chrono::high_resolution_clock::now();
        bool wasPressedPreviousFrame = false;
		while (!appWindow.isClosing())
		{
            if (glfwGetKey(appWindow.getGLFWwindow(), GLFW_KEY_J) == GLFW_PRESS && !wasPressedPreviousFrame)
            {
                wasPressedPreviousFrame = true;
                isWireframe = !isWireframe;
                simpleRenderSystem.setWireframe(isWireframe, appRenderer.getSwapChainRenderPass());
            }
            else if(glfwGetKey(appWindow.getGLFWwindow(), GLFW_KEY_J) != GLFW_PRESS)
            {
                wasPressedPreviousFrame = false;
            }
            glfwPollEvents();
            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime =
                std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(appWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);;
            float aspect = appRenderer.getAspectRatio();
            
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 1000.f);
			if (auto commandBuffer = appRenderer.beginFrame()) {
				appRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
				appRenderer.endSwapChainRenderPass(commandBuffer);
				appRenderer.endFrame();
			}
		}
		vkDeviceWaitIdle(appDevice.device());
	}


    std::unique_ptr<AppModel> DefaultApp::createChunk(AppDevice & device, glm::vec3 offset, int chunkX, int chunkZ) {
        std::vector<AppModel::Vertex> vertices{};
        std::vector<float> values{};
        for (int x = 0; x < _CHUNK_SIZE_; x++)
        {
            for (int z = 0; z < _CHUNK_SIZE_; z++)
            {
                double perlinValue1 = perlin.GetNoiseValue( 0.04 * (x+3000) + (chunkX*_CHUNK_SIZE_), 0.5, 0.04 * (z+3000) + (chunkZ * _CHUNK_SIZE_));
                double perlinValue2 = perlin.GetNoiseValue(0.01 * (x-3333) + (chunkX * _CHUNK_SIZE_), 0.5, 0.01 * (z-3444) + (chunkZ * _CHUNK_SIZE_));
                double perlinValue3 = perlin.GetNoiseValue(0.3 * x + (chunkX * _CHUNK_SIZE_), 0.5, 0.3 * z + (chunkZ * _CHUNK_SIZE_));
                double perlinValue4 = perlin.GetNoiseValue(0.005 * (x + 23233) + (chunkX * _CHUNK_SIZE_), 0.5, 0.01 * (z + 3334) + (chunkZ * _CHUNK_SIZE_));
                double finalPerlin = 50*(perlinValue1 * 0.25 + perlinValue2 * 0.125 + perlinValue3 * 0.025 + perlinValue4*0.6);
                values.push_back((float)finalPerlin);
            }
        }
    
        for (int z = 0; z < _CHUNK_SIZE_-1; z++)
        {
            for (int x = 0; x < _CHUNK_SIZE_-1; x++)
            {
                double perlinColor1 = (perlin.GetNoiseValue(0.06 * (x + 30000) + (chunkX * _CHUNK_SIZE_), 0.5, 0.06 * (z + 30000) + (chunkZ * _CHUNK_SIZE_)) + 1.0f)/2.f;
                double perlinColor2 = (perlin.GetNoiseValue(0.04 * (x - 30000) + (chunkX * _CHUNK_SIZE_), 0.5, 0.04 * (z - 30000) + (chunkZ * _CHUNK_SIZE_)) + 1.0f) / 2.f;
                double perlinColor3 = (perlin.GetNoiseValue(0.01 * (x - 50000) + (chunkX * _CHUNK_SIZE_), 0.5, 0.01 * (z - 50000) + (chunkZ * _CHUNK_SIZE_)) + 1.0f) / 4.f;
                auto col1 = 1.f-((((values[x + z * _CHUNK_SIZE_] + values[x + 1 + z * _CHUNK_SIZE_] + values[x + (z + 1) * _CHUNK_SIZE_])/3.f)+25.f)/50.f);
                auto col2 = 1.f- ((((values[x + 1 + z * _CHUNK_SIZE_] + values[x + 1 + (z + 1) * _CHUNK_SIZE_] + values[x + (z + 1) * _CHUNK_SIZE_]) / 3.f) + 25.f) / 50.f);
               
                vertices.push_back({ {(float)x +        (chunkX * _CHUNK_SIZE_), values[x +    z     * _CHUNK_SIZE_],(float)z + (chunkZ * _CHUNK_SIZE_)}, {perlinColor2 , col1/2+ perlinColor3,perlinColor1 }});
                vertices.push_back({ {(float)x + 1.f +  (chunkX * _CHUNK_SIZE_), values[x + 1+ z     * _CHUNK_SIZE_],(float)z +       (chunkZ * _CHUNK_SIZE_)}, {perlinColor2 , col1/2+ perlinColor3, perlinColor1 } });
                vertices.push_back({ {(float)x +        (chunkX * _CHUNK_SIZE_), values[x +    (z+1)  * _CHUNK_SIZE_],(float)z + 1.f + (chunkZ * _CHUNK_SIZE_)}, {perlinColor2 , col1/2+ perlinColor3, perlinColor1 } });
                vertices.push_back({ {(float)x + 1.f +  (chunkX * _CHUNK_SIZE_), values[x + 1+ z     * _CHUNK_SIZE_],(float)z +       (chunkZ * _CHUNK_SIZE_)}, {perlinColor2 , col2/2+ perlinColor3, perlinColor1 } });
                vertices.push_back({ {(float)x + 1.f +  (chunkX * _CHUNK_SIZE_), values[x + 1+(z + 1)* _CHUNK_SIZE_],(float)z + 1.f + (chunkZ * _CHUNK_SIZE_)}, {perlinColor2 , col2/2+ perlinColor3, perlinColor1 } });
                vertices.push_back({ {(float)x +        (chunkX * _CHUNK_SIZE_), values[x +   (z + 1)* _CHUNK_SIZE_],(float)z + 1.f + (chunkZ * _CHUNK_SIZE_)}, {perlinColor2 , col2/2+ perlinColor3, perlinColor1 } });
            }
        }
        for (auto& v : vertices) {
            v.position += offset;
        }
        return std::make_unique<AppModel>(device, vertices);
    }

    void DefaultApp::loadGameObjects() {
        std::shared_ptr<AppModel> appModel = createChunk(appDevice, { .0f, .0f, .0f }, 0 ,0);
        auto cube = AppGameObject::createGameObject();
        cube.model = appModel;
        cube.transform.translation = { -200.0f, 5.f, -200.0f };
        cube.transform.scale = { 1.f, 1.f, 1.f };
        gameObjects.push_back(std::move(cube));
	}
	
}