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

    static const float PROJECTION_RENDER_DISTANCE = 1000.f;
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
            
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.01f, PROJECTION_RENDER_DISTANCE);
			if (auto commandBuffer = appRenderer.beginFrame()) {
				appRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
				appRenderer.endSwapChainRenderPass(commandBuffer);
				appRenderer.endFrame();
			}
		}
		vkDeviceWaitIdle(appDevice.device());
	}

    std::unique_ptr<AppModel> DefaultApp::createCubeModel(AppDevice& device, glm::vec3 offset) {
        AppModel::Builder modelBuilder{};
        modelBuilder.vertices = {
            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        };

        for (auto& v : modelBuilder.vertices) {
            v.position += offset;
        }
        modelBuilder.indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                          12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };

        return std::make_unique<AppModel>(device, modelBuilder);
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
               
                glm::vec3 p1 = { 
                    (float)x + (chunkX * _CHUNK_SIZE_),
                    values[x + z * _CHUNK_SIZE_],
                    (float)z + (chunkZ * _CHUNK_SIZE_)
                };


                glm::vec3 p2 = {
                    (float)x + 1.f + (chunkX * _CHUNK_SIZE_),
                    values[x + 1 + z * _CHUNK_SIZE_],
                    (float)z + (chunkZ * _CHUNK_SIZE_)
                };

                glm::vec3 p3 = {
                    (float)x + (chunkX * _CHUNK_SIZE_),
                    values[x + (z + 1) * _CHUNK_SIZE_],
                    (float)z + 1.f + (chunkZ * _CHUNK_SIZE_)
                };

                glm::vec3 p4 = {
                    (float)x + 1.f + (chunkX * _CHUNK_SIZE_),
                    values[x + 1 + (z + 1) * _CHUNK_SIZE_],
                    (float)z + 1.f + (chunkZ * _CHUNK_SIZE_)
                };

                glm::vec3 A1 = p2 - p1;
                glm::vec3 B1 = p3 - p1;
                //Nx = Ay * Bz - Az * By
                float Nx1 = (A1.y * B1.z) - (A1.z * B1.y);
                //Ny = Az * Bx - Ax * Bz
                float Ny1 = (A1.z * B1.x) - (A1.x * B1.z);
                //Nz = Ax * By - Ay * Bx
                float Nz1 = (A1.x * B1.y) - (A1.y * B1.x);

                glm::vec3 A2 = p4 - p2;
                glm::vec3 B2 = p3 - p2;
           
                //Nx = Ay * Bz - Az * By
                float Nx2 =(A2.y * B2.z) - (A2.z * B2.y);
                //Ny = Az * Bx - Ax * Bz
                float Ny2 =(A2.z * B2.x) - (A2.x * B2.z);
                //Nz = Ax * By - Ay * Bx
                float Nz2 =(A2.x * B2.y) - (A2.y * B2.x);

                vertices.push_back({ {(float)x +        (chunkX * _CHUNK_SIZE_), values[x +    z     * _CHUNK_SIZE_],(float)z + (chunkZ * _CHUNK_SIZE_)},{0.f , 1.f ,col2 }, {Nx1,Ny1,Nz1} });
                vertices.push_back({ {(float)x + 1.f +  (chunkX * _CHUNK_SIZE_), values[x + 1+ z     * _CHUNK_SIZE_],(float)z +       (chunkZ * _CHUNK_SIZE_)}, {0.f , 1.f ,col2 }, {Nx1,Ny1,Nz1} });
                vertices.push_back({ {(float)x +        (chunkX * _CHUNK_SIZE_), values[x +    (z+1)  * _CHUNK_SIZE_],(float)z + 1.f + (chunkZ * _CHUNK_SIZE_)}, {0.f , 1.f ,col2 }, {Nx1,Ny1,Nz1} });
                vertices.push_back({ {(float)x + 1.f +  (chunkX * _CHUNK_SIZE_), values[x + 1+ z     * _CHUNK_SIZE_],(float)z +       (chunkZ * _CHUNK_SIZE_)}, {0.f , 1.f ,col1 }, {Nx2,Ny2,Nz2} });
                vertices.push_back({ {(float)x + 1.f +  (chunkX * _CHUNK_SIZE_), values[x + 1+(z + 1)* _CHUNK_SIZE_],(float)z + 1.f + (chunkZ * _CHUNK_SIZE_)}, {0.f , 1.f ,col1 }, {Nx2,Ny2,Nz2} });
                vertices.push_back({ {(float)x +        (chunkX * _CHUNK_SIZE_), values[x +   (z + 1)* _CHUNK_SIZE_],(float)z + 1.f + (chunkZ * _CHUNK_SIZE_)}, {0.f , 1.f ,col1 }, {Nx2,Ny2,Nz2} });
                //vertices.push_back({ {(float)x + (chunkX * _CHUNK_SIZE_), values[x + z * _CHUNK_SIZE_],(float)z + (chunkZ * _CHUNK_SIZE_)}, {0.f , 1.f ,0.f } });
                //vertices.push_back({ {(float)x + 1.f + (chunkX * _CHUNK_SIZE_), values[x + 1 + z * _CHUNK_SIZE_],(float)z + (chunkZ * _CHUNK_SIZE_)},  {0.f , 1.f ,0.f } });
                //vertices.push_back({ {(float)x + (chunkX * _CHUNK_SIZE_), values[x + (z + 1) * _CHUNK_SIZE_],(float)z + 1.f + (chunkZ * _CHUNK_SIZE_)},  {0.f , 1.f ,0.f } });
                //vertices.push_back({ {(float)x + 1.f + (chunkX * _CHUNK_SIZE_), values[x + 1 + z * _CHUNK_SIZE_],(float)z + (chunkZ * _CHUNK_SIZE_)},  {0.f , 1.f ,0.f } });
                //vertices.push_back({ {(float)x + 1.f + (chunkX * _CHUNK_SIZE_), values[x + 1 + (z + 1) * _CHUNK_SIZE_],(float)z + 1.f + (chunkZ * _CHUNK_SIZE_)},  {0.f , 1.f ,0.f } });
                //vertices.push_back({ {(float)x + (chunkX * _CHUNK_SIZE_), values[x + (z + 1) * _CHUNK_SIZE_],(float)z + 1.f + (chunkZ * _CHUNK_SIZE_)},  {0.f , 1.f ,0.f } });
            }
        }
        for (auto& v : vertices) {
            v.position += offset;
        }
        AppModel::Builder modelBuilder{};
        modelBuilder.vertices = vertices;
        return std::make_unique<AppModel>(device, modelBuilder);
    }

    void DefaultApp::loadGameObjects() {
        std::shared_ptr<AppModel> appModel = createChunk(appDevice, { .0f, .0f, .0f }, 0 ,0);
        auto cube = AppGameObject::createGameObject();
        cube.model = appModel;
        cube.transform.translation = { -200.0f, 5.f, -200.0f };
        cube.transform.scale = { 1.f, 1.f, 1.f };
        gameObjects.push_back(std::move(cube));
        std::shared_ptr<AppModel> appModel2 = createCubeModel(appDevice, { .0f, .0f, .0f });
        auto cube2 = AppGameObject::createGameObject();
        cube2.model = appModel2;
        cube2.transform.translation = { -200.0f, 5.f, -200.0f };
        cube2.transform.scale = { 1.f, 1.f, 1.f };
        gameObjects.push_back(std::move(cube2));
        appModel = AppModel::createModelFromFile(appDevice, "models/smooth_vase.obj");
        auto tree = AppGameObject::createGameObject();
        tree.model = appModel;
        tree.transform.translation = { .5f, .5f, 2.5f };
        tree.transform.scale = { 3.f, 1.5f, 3.f };
        gameObjects.push_back(std::move(tree));
	}
	
}