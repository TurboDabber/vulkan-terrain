#pragma once
#pragma once

#include "app_model.hpp"
#include <glm/gtc/matrix_transform.hpp>
// std
#include <memory>

namespace app {

    struct TransformComponent {
        glm::vec3 translation{};
        glm::vec3 scale{ 1.f, 1.f, 1.f };
        glm::vec3 rotation{};

        glm::mat4 mat4();

        glm::mat3 normalMatrix();
    };

    class AppGameObject {
    public:
        using id_t = unsigned int;

        static AppGameObject createGameObject() {
            static id_t currentId = 0;
            return AppGameObject{ currentId++ };
        }

        AppGameObject(const AppGameObject&) = delete;
        AppGameObject& operator=(const AppGameObject&) = delete;
        AppGameObject(AppGameObject&&) = default;
        AppGameObject& operator=(AppGameObject&&) = default;

        id_t getId() { return id; }

        std::shared_ptr<AppModel> model{};
        glm::vec3 color{};
        TransformComponent transform{};

    private:
        AppGameObject(id_t objId) : id{ objId } {}

        id_t id;
    };
}  // namespace app