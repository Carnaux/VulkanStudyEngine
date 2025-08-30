#pragma once

#include "vse_model.hpp"

// std
#include <memory>

namespace vse {

struct Transform2dComponent {
  glm::vec2 translation{};
  glm::vec2 scale{1.0f, 1.0f};
  float rotation;

  glm::mat2 mat2() {
    const float s = glm::sin(rotation);
    const float c = glm::cos(rotation);
    glm::mat2 rotMat{{c, s}, {-s, c}};

    glm::mat2 scaleMat{{scale.x, .0f}, {.0f, scale.y}};
    return rotMat * scaleMat;
  }
};

class VseGameObject {
 public:
  using id_t = unsigned int;

  static VseGameObject createGameObject() {
    static id_t currentId = 0;
    return VseGameObject{currentId++};
  }

  VseGameObject(const VseGameObject &) = delete;
  VseGameObject &operator=(const VseGameObject &) = delete;
  VseGameObject(VseGameObject &&) = default;
  VseGameObject &operator=(VseGameObject &&) = default;

  id_t getId() { return id; }

  std::shared_ptr<VseModel> model{};
  glm::vec3 color{};
  Transform2dComponent transform2d{};

 private:
  VseGameObject(id_t objId) : id{objId} {}

  id_t id;
};
}  // namespace vse