#pragma once

#include "vse_device.hpp"
#include "vse_game_object.hpp"
#include "vse_renderer.hpp"
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
  void loadGameObjects();

  VseWindow vseWindow{WIDTH, HEIGHT, "VSE Application"};
  VseDevice vseDevice{vseWindow};
  VseRenderer vseRenderer{vseWindow, vseDevice};

  std::vector<VseGameObject> gameObjects;
};

}  // namespace vse