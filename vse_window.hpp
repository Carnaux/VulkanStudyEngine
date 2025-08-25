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
  bool wasWindowResized() { return frameBufferResized; }
  void resetWindowResizedFlag() { frameBufferResized = false; }

  void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

 private:
  static void frameBufferResizeCallback(GLFWwindow *window, int width,
                                        int height);
  void initWindow();

  int width;
  int height;
  bool frameBufferResized = false;

  std::string windowName;
  GLFWwindow *window;
};

}  // namespace vse