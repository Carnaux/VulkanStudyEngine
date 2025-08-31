#include "vse_app.hpp"

#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <stdexcept>

namespace vse {

VseApp::VseApp() { loadGameObjects(); }

VseApp::~VseApp() {}

void VseApp::run() {
  SimpleRenderSystem simpleRenderSystem{vseDevice,
                                        vseRenderer.getSwapChainRenderPass()};
  while (!vseWindow.ShouldClose()) {
    glfwPollEvents();

    if (auto commandBuffer = vseRenderer.beginFrame()) {
      vseRenderer.beginSwapChainRenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
      vseRenderer.endSwapChainRenderPass(commandBuffer);
      vseRenderer.endFrame();
    }
  }

  vkDeviceWaitIdle(vseDevice.device());
}

std::unique_ptr<VseModel> createCubeModel(VseDevice& device, glm::vec3 offset) {
  std::vector<VseModel::Vertex> vertices{
      // left face (white, x = -0.5)
      {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

      // right face (yellow, x = +0.5)
      {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
      {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
      {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},

      // top face (red, y = +0.5)
      {{-.5f, .5f, -.5f}, {.9f, .6f, .1f}},
      {{.5f, .5f, .5f}, {.9f, .6f, .1f}},
      {{.5f, .5f, -.5f}, {.9f, .6f, .1f}},
      {{-.5f, .5f, -.5f}, {.9f, .6f, .1f}},
      {{-.5f, .5f, .5f}, {.9f, .6f, .1f}},
      {{.5f, .5f, .5f}, {.9f, .6f, .1f}},

      // bottom face (orange, y = -0.5)
      {{-.5f, -.5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, -.5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, -.5f, .5f}, {.8f, .1f, .1f}},
      {{-.5f, -.5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, -.5f, .5f}, {.8f, .1f, .1f}},
      {{-.5f, -.5f, .5f}, {.8f, .1f, .1f}},

      // nose face (blue, z = +0.5)
      {{-.5f, -.5f, .5f}, {.1f, .1f, .8f}},
      {{.5f, .5f, .5f}, {.1f, .1f, .8f}},
      {{.5f, -.5f, .5f}, {.1f, .1f, .8f}},
      {{-.5f, -.5f, .5f}, {.1f, .1f, .8f}},
      {{-.5f, .5f, .5f}, {.1f, .1f, .8f}},
      {{.5f, .5f, .5f}, {.1f, .1f, .8f}},

      // tail face (green, z = -0.5)
      {{-.5f, -.5f, -.5f}, {.1f, .8f, .1f}},
      {{.5f, -.5f, -.5f}, {.1f, .8f, .1f}},
      {{.5f, .5f, -.5f}, {.1f, .8f, .1f}},
      {{-.5f, -.5f, -.5f}, {.1f, .8f, .1f}},
      {{.5f, .5f, -.5f}, {.1f, .8f, .1f}},
      {{-.5f, .5f, -.5f}, {.1f, .8f, .1f}},

  };
  for (auto& v : vertices) {
    v.position += offset;
  }
  return std::make_unique<VseModel>(device, vertices);
}

void VseApp::loadGameObjects() {
  std::shared_ptr<VseModel> vseModel =
      createCubeModel(vseDevice, {.0f, .0f, .0f});

  auto cube = VseGameObject::createGameObject();
  cube.model = vseModel;
  cube.transform.translation = {.0f, .0f, .5f};
  cube.transform.scale = {.5f, .5f, .5f};

  gameObjects.push_back(std::move(cube));
}

}  // namespace vse