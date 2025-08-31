#include "vse_renderer.hpp"

// std
#include <array>
#include <stdexcept>

namespace vse {

VseRenderer::VseRenderer(VseWindow &window, VseDevice &device)
    : vseWindow{window}, vseDevice{device} {
  recreateSwapChain();
  createCommandBuffers();
}

VseRenderer::~VseRenderer() { freeCommandBuffers(); }

void VseRenderer::recreateSwapChain() {
  auto extent = vseWindow.getExtent();
  while (extent.width == 0 || extent.height == 0) {
    extent = vseWindow.getExtent();
    glfwWaitEvents();
  }
  vseSwapChain = nullptr;
  vkDeviceWaitIdle(vseDevice.device());
  if (vseSwapChain == nullptr) {
    vseSwapChain = std::make_unique<VseSwapChain>(vseDevice, extent);
  } else {
    std::shared_ptr<VseSwapChain> oldSwapChain = std::move(vseSwapChain);
    vseSwapChain =
        std::make_unique<VseSwapChain>(vseDevice, extent, oldSwapChain);

    if (!oldSwapChain->compareSwapChainFormats(*vseSwapChain.get())) {
      throw std::runtime_error("Swap chain image(or depth) format has changed");
    }
  }
}

void VseRenderer::createCommandBuffers() {
  commandBuffers.resize(VseSwapChain::MAX_FRAMES_IN_FLIGHT);

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = vseDevice.getCommandPool();
  allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

  if (vkAllocateCommandBuffers(vseDevice.device(), &allocInfo,
                               commandBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("Failed to allocate command buffers");
  }
}

void VseRenderer::freeCommandBuffers() {
  vkFreeCommandBuffers(vseDevice.device(), vseDevice.getCommandPool(),
                       static_cast<float>(commandBuffers.size()),
                       commandBuffers.data());
  commandBuffers.clear();
}

VkCommandBuffer VseRenderer::beginFrame() {
  assert(!isFrameStarted && "Can't call begin frame while already in progress");

  auto result = vseSwapChain->acquireNextImage(&currentImageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
    return nullptr;
  }

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("Failed to acquire swap chain image!");
  }

  isFrameStarted = true;

  auto commandBuffer = getCurrentCommandBuffer();
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("Failed to begin recording command buffer");
  }

  return commandBuffer;
}

void VseRenderer::endFrame() {
  assert(isFrameStarted &&
         "Can't call endFrame while frame is not in progress");
  auto commandBuffer = getCurrentCommandBuffer();

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("Failed to record command buffer");
  }

  auto result =
      vseSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      vseWindow.wasWindowResized()) {
    vseWindow.resetWindowResizedFlag();
    recreateSwapChain();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to acquire swap chain image!");
  }

  isFrameStarted = false;
  currentFrameIndex =
      (currentFrameIndex + 1) % VseSwapChain::MAX_FRAMES_IN_FLIGHT;
}
void VseRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
  assert(isFrameStarted &&
         "Can't call beginSwapChainRenderPass while frame is not in progress");
  assert(commandBuffer == getCurrentCommandBuffer() &&
         "Can't begin render pass on command buffer from a different frame");

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = vseSwapChain->getRenderPass();
  renderPassInfo.framebuffer = vseSwapChain->getFrameBuffer(currentImageIndex);

  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = vseSwapChain->getSwapChainExtent();

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = {
      0.01f,
      0.01f,
      0.01f,
      1.0f,
  };
  clearValues[1].depthStencil = {1.0f, 0};
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(vseSwapChain->getSwapChainExtent().width);
  viewport.height =
      static_cast<float>(vseSwapChain->getSwapChainExtent().height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  VkRect2D scissor{{0, 0}, vseSwapChain->getSwapChainExtent()};
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}
void VseRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
  assert(isFrameStarted &&
         "Can't call endSwapChainRenderPass while frame is not in progress");
  assert(commandBuffer == getCurrentCommandBuffer() &&
         "Can't end render pass on command buffer from a different frame");

  vkCmdEndRenderPass(commandBuffer);
}

}  // namespace vse