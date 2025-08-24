#include "vse_app.hpp"

// std
#include <array>
#include <stdexcept>

namespace vse {

VseApp::VseApp() {
  loadModels();
  createPipelineLayout();
  createPipeline();
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
  std::vector<VseModel::Vertex> vertices{
      {{0.0f, -0.5f}}, {{0.5f, 0.5f}}, {{-0.5f, 0.5f}}};

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
  auto pipelineConfig = VsePipeline::defaultPipelineConfigInfo(
      vseSwapChain.width(), vseSwapChain.height());
  pipelineConfig.renderPass = vseSwapChain.getRenderPass();
  pipelineConfig.pipelineLayout = pipelineLayout;
  vsePipeline = std::make_unique<VsePipeline>(
      vseDevice, "shaders/simple_shader.vert.spv",
      "shaders/simple_shader.frag.spv", pipelineConfig);
}

void VseApp::createCommandBuffers() {
  commandBuffers.resize(vseSwapChain.imageCount());

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = vseDevice.getCommandPool();
  allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

  if (vkAllocateCommandBuffers(vseDevice.device(), &allocInfo,
                               commandBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("Failed to allocate command buffers");
  }

  for (int i = 0; i < commandBuffers.size(); i++) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("Failed to begin recording command buffer");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vseSwapChain.getRenderPass();
    renderPassInfo.framebuffer = vseSwapChain.getFrameBuffer(i);

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = vseSwapChain.getSwapChainExtent();

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

    vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    vsePipeline->bind(commandBuffers[i]);
    vseModel->bind(commandBuffers[i]);
    vseModel->draw(commandBuffers[i]);

    vkCmdEndRenderPass(commandBuffers[i]);
    if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("Failed to record command buffer");
    }
  }
}

void VseApp::drawFrame() {
  uint32_t imageIndex;
  auto result = vseSwapChain.acquireNextImage(&imageIndex);

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("Failed to acquire swap chain image!");
  }

  result = vseSwapChain.submitCommandBuffers(&commandBuffers[imageIndex],
                                             &imageIndex);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to acquire swap chain image!");
  }
}

}  // namespace vse