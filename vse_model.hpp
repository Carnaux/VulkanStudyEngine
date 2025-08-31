#pragma once

#include "vse_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <vector>

namespace vse {

class VseModel {
 public:
  struct Vertex {
    glm::vec3 position;
    glm::vec3 color;

    static std::vector<VkVertexInputBindingDescription>
    getBindingDescriptions();
    static std::vector<VkVertexInputAttributeDescription>
    getAttributeDescriptions();
  };

  VseModel(VseDevice &device, const std::vector<Vertex> &vertices);
  ~VseModel();

  VseModel(VseModel &&) = delete;
  VseModel &operator=(VseModel &&) = delete;

  void bind(VkCommandBuffer commandBuffer);
  void draw(VkCommandBuffer commandBuffer);

 private:
  VseDevice &vseDevice;
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  uint32_t vertexCount;

  void createVertexBuffers(const std::vector<Vertex> &vertices);
};
}  // namespace vse
