#pragma once

#include "vse_device.hpp"
#include "vse_model.hpp"
#include "vse_pipeline.hpp"
#include "vse_swap_chain.hpp"
#include "vse_window.hpp"

// std
#include <memory>
#include <vector>

namespace vse {

class VseApp {
 public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  VseApp();
  ~VseApp();

  VseApp(const VseApp &) = delete;
  VseApp &operator=(const VseApp &) = delete;

  void run();

 private:
  void loadModels();
  void createPipelineLayout();
  void createPipeline();
  void createCommandBuffers();
  void freeCommandBuffers();
  void drawFrame();
  void recreateSwapChain();
  void recordCommandBuffer(int imageIndex);

  VseWindow vseWindow{WIDTH, HEIGHT, "VSE Application"};
  VseDevice vseDevice{vseWindow};
  std::unique_ptr<VseSwapChain> vseSwapChain;
  std::unique_ptr<VsePipeline> vsePipeline;
  VkPipelineLayout pipelineLayout;
  std::vector<VkCommandBuffer> commandBuffers;
  std::unique_ptr<VseModel> vseModel;
};

}  // namespace vse