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

void VseApp::loadGameObjects() {
  std::vector<VseModel::Vertex> vertices{{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                         {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                         {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

  auto vseModel = std::make_shared<VseModel>(vseDevice, vertices);

  auto triangle = VseGameObject::createGameObject();
  triangle.model = vseModel;
  triangle.color = {.1f, .8f, .1f};
  triangle.transform2d.translation.x = .2f;
  triangle.transform2d.scale = {2.f, .5f};
  triangle.transform2d.rotation = .25f * glm::two_pi<float>();

  gameObjects.push_back(std::move(triangle));
}

}  // namespace vse