#ifndef H_4RC_REACT0R_H
#define H_4RC_REACT0R_H

#include "scribe.hpp"
#include "Window.hpp"
#include "arsenal.hpp"

class ArcReactor
{
private:
  std::string appName = "Vulkan_Forge";
  forge::Window win{WIDTH, HEIGHT, appName};
  static constexpr uint32_t WIDTH = 1366;
  static constexpr uint32_t HEIGHT = 16*WIDTH/9;

  void input();
  void update();
  void render();

public:
  void pump();
  ArcReactor();
  ~ArcReactor();
};

#endif




