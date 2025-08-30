#pragma once

#include "vse_device.hpp"
#include "vse_game_object.hpp"
#include "vse_pipeline.hpp"

// std
#include <memory>
#include <vector>

namespace vse {

class SimpleRenderSystem {
 public:
  SimpleRenderSystem(VseDevice &device, VkRenderPass renderPass);
  ~SimpleRenderSystem();

  SimpleRenderSystem(const SimpleRenderSystem &) = delete;
  SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

  void renderGameObjects(VkCommandBuffer commandBuffer,
                         std::vector<VseGameObject> &gameObjects);

 private:
  void createPipelineLayout();
  void createPipeline(VkRenderPass renderPass);

  VseDevice &vseDevice;

  std::unique_ptr<VsePipeline> vsePipeline;
  VkPipelineLayout pipelineLayout;
};

}  // namespace vse