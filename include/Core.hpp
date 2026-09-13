#ifndef H_C0R3_H
#define H_C0R3_H

#include "scribe.hpp"
#include "Window.hpp"
#include "arsenal.hpp"
#include "aether.hpp"
#include "crypt.hpp"
#include "sanctum.hpp"
#include "Pipe.hpp"

class Core
{
private:
  void input();
  void update();
  void render();

  std::string appName = "Vulkan_Forge";
  forge::Window win{WIDTH, HEIGHT, appName};
  static constexpr uint32_t HEIGHT = 600;
  static constexpr uint32_t WIDTH = 16*HEIGHT/9;
  
  Pipe simplePipe;

public:
  void pump();
  Core();
  ~Core();
};

#endif




