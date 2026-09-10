#include "Core.hpp"

int main()
{
  Core core;
  core.pump();
}

Core::Core()
{
  forge::arsenal::evoke(&appName, win.getWindowPointer(), WIDTH, HEIGHT);
}

Core::~Core()
{
  vkDeviceWaitIdle(forge::arsenal::device);

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


