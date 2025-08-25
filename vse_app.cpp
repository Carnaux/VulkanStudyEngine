#include "vse_app.hpp"

// std
#include <array>
#include <stdexcept>

namespace vse {

VseApp::VseApp() {
  loadModels();
  createPipelineLayout();
  recreateSwapChain();
  createCommandBuffers();
}

VseApp::~VseApp() {
  vkDestroyPipelineLayout(vseDevice.device(), pipelineLayout, nullptr);
}

void VseApp::run() {
  while (!vseWindow.ShouldClose()) {
    glfwPollEvents();
    drawFrame();
  }

  vkDeviceWaitIdle(vseDevice.device());
}

void VseApp::loadModels() {
  std::vector<VseModel::Vertex> vertices{{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                         {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                         {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

  vseModel = std::make_unique<VseModel>(vseDevice, vertices);
}

void VseApp::createPipelineLayout() {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr;

  if (vkCreatePipelineLayout(vseDevice.device(), &pipelineLayoutInfo, nullptr,
                             &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

void VseApp::createPipeline() {
  assert(vseSwapChain != nullptr && "Cannot create pipeline before swapchain");
  assert(pipelineLayout != nullptr &&
         "Cannot create pipeline before pipeline layout");

  PipelineConfigInfo pipelineConfig{};
  VsePipeline::defaultPipelineConfigInfo(pipelineConfig);

  pipelineConfig.renderPass = vseSwapChain->getRenderPass();
  pipelineConfig.pipelineLayout = pipelineLayout;
  vsePipeline = std::make_unique<VsePipeline>(
      vseDevice, "shaders/simple_shader.vert.spv",
      "shaders/simple_shader.frag.spv", pipelineConfig);
}

void VseApp::recreateSwapChain() {
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
    vseSwapChain = std::make_unique<VseSwapChain>(vseDevice, extent,
                                                  std::move(vseSwapChain));
    if (vseSwapChain->imageCount() != commandBuffers.size()) {
      freeCommandBuffers();
      createCommandBuffers();
    }
  }

  createPipeline();
}

void VseApp::createCommandBuffers() {
  commandBuffers.resize(vseSwapChain->imageCount());

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

void VseApp::freeCommandBuffers() {
  vkFreeCommandBuffers(vseDevice.device(), vseDevice.getCommandPool(),
                       static_cast<float>(commandBuffers.size()),
                       commandBuffers.data());
  commandBuffers.clear();
}

void VseApp::recordCommandBuffer(int imageIndex) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to begin recording command buffer");
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = vseSwapChain->getRenderPass();
  renderPassInfo.framebuffer = vseSwapChain->getFrameBuffer(imageIndex);

  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = vseSwapChain->getSwapChainExtent();

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = {
      0.1f,
      0.1f,
      0.1f,
      1.0f,
  };

  clearValues[1].depthStencil = {0.1f, 0};
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo,
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
  vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
  vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

  vsePipeline->bind(commandBuffers[imageIndex]);
  vseModel->bind(commandBuffers[imageIndex]);
  vseModel->draw(commandBuffers[imageIndex]);

  vkCmdEndRenderPass(commandBuffers[imageIndex]);
  if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
    throw std::runtime_error("Failed to record command buffer");
  }
}

void VseApp::drawFrame() {
  uint32_t imageIndex;
  auto result = vseSwapChain->acquireNextImage(&imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
    return;
  }

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("Failed to acquire swap chain image!");
  }

  recordCommandBuffer(imageIndex);
  result = vseSwapChain->submitCommandBuffers(&commandBuffers[imageIndex],
                                              &imageIndex);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      vseWindow.wasWindowResized()) {
    vseWindow.resetWindowResizedFlag();
    recreateSwapChain();
    return;
  }
  if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to acquire swap chain image!");
  }
}

}  // namespace vse