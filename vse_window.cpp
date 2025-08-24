#include "vse_window.hpp"

// std
#include <stdexcept>

namespace vse {

VseWindow::VseWindow(int w, int h, std::string const &name)
    : width{w}, height{h}, windowName{name} {
  initWindow();
}

VseWindow::~VseWindow() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void VseWindow::initWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  window =
      glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
  if (!window) {
    throw std::runtime_error("Failed to create GLFW window");
  }
}

void VseWindow::createWindowSurface(VkInstance instance,
                                    VkSurfaceKHR *surface) {
  if (glfwCreateWindowSurface(instance, window, nullptr, surface) !=
      VK_SUCCESS) {
    throw std::runtime_error("filed to create window surface");
  }
}

}  // namespace vse