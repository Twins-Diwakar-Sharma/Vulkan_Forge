#ifndef H_C0R3_H
#define H_C0R3_H

#include "scribe.hpp"
#include "Window.hpp"
#include "arsenal.hpp"
#include "aether.hpp"

class Core
{
private:
  std::string appName = "Vulkan_Forge";
  forge::Window win{WIDTH, HEIGHT, appName};
  static constexpr uint32_t WIDTH = 800;
  static constexpr uint32_t HEIGHT = 16*WIDTH/9;

  void input();
  void update();
  void render();

public:
  void pump();
  Core();
  ~Core();
};

#endif




