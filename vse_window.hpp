#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace vse {

class VseWindow {
 public:
  VseWindow(int w, int h, const std::string &name);
  ~VseWindow();

  VseWindow(const VseWindow &) = delete;
  VseWindow &operator=(const VseWindow &) = delete;

  bool ShouldClose() { return glfwWindowShouldClose(window); }

  VkExtent2D getExtent() {
    return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
  }

  void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

 private:
  void initWindow();

  const int width;
  const int height;

  std::string windowName;
  GLFWwindow *window;
};

}  // namespace vse