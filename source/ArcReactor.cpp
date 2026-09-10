#include "ArcReactor.hpp"

ArcReactor::ArcReactor()
{
  forge::arsenal::evoke(&appName, win.getWindowPointer(), WIDTH, HEIGHT);
}

ArcReactor::~ArcReactor()
{
  vkDeviceWaitIdle(forge::arsenal::device);

  forge::arsenal::eradicate();
}

void ArcReactor::pump()
{
  while(!win.shouldClose())
  {
    input();
    update();
    render();
  }
}

void ArcReactor::input()
{
  win.handleKey(); 
  glfwPollEvents();
}


void ArcReactor::update()
{

}


void ArcReactor::render()
{

}


