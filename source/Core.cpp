#include "Core.hpp"

int main()
{
  Core core;
  core.pump();
}

Core::Core()
{
  forge::arsenal::evoke(&appName, win.getWindowPointer(), WIDTH, HEIGHT);
  forge::aether::frames::evoke();
  forge::aether::swap::evoke(win.getWindowPointer());
}

Core::~Core()
{
  vkDeviceWaitIdle(forge::arsenal::device);
  
  forge::aether::swap::eradicate();
  forge::aether::frames::eradicate();
  forge::arsenal::eradicate();
}

void Core::pump()
{
  while(!win.shouldClose())
  {
    input();
    update();
    render();
  }
}

void Core::input()
{
  win.handleKey(); 
  glfwPollEvents();
}


void Core::update()
{

}


void Core::render()
{

}


