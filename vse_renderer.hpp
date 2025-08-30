#pragma once

#include "vse_device.hpp"
#include "vse_swap_chain.hpp"
#include "vse_window.hpp"

// std
#include <cassert>
#include <memory>
#include <vector>

namespace vse {

class VseRenderer {
 public:
  VseRenderer(VseWindow &window, VseDevice &device);
  ~VseRenderer();

  VseRenderer(const VseRenderer &) = delete;
  VseRenderer &operator=(const VseRenderer &) = delete;

  VkRenderPass getSwapChainRenderPass() const {
    return vseSwapChain->getRenderPass();
  }
  bool isFrameInProgress() const { return isFrameStarted; }

  VkCommandBuffer getCurrentCommandBuffer() const {
    assert(isFrameStarted &&
           "Cannot get command buffer when frame not in progress");
    return commandBuffers[currentFrameIndex];
  }

  int getFrameIndex() const {
    assert(isFrameStarted &&
           "Cannot get frame index when frame not in progress");
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

  VseWindow &vseWindow;
  VseDevice &vseDevice;
  std::unique_ptr<VseSwapChain> vseSwapChain;
  std::vector<VkCommandBuffer> commandBuffers;

  uint32_t currentImageIndex{0};
  int currentFrameIndex{0};
  bool isFrameStarted{false};
};

}  // namespace vse