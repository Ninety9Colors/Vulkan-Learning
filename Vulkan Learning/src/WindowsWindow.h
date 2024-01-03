#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_EXPOSE_NATIVE_WIN32

#include "Headers.h"

#include <glfw3native.h>

class WindowsWindow {
public:
  WindowsWindow(bool debug);
  ~WindowsWindow();

  static void error_callback(int error, const char* description);
  void on_update();
  void create_glfw_window();
  void create_surface(const vk::Instance& instance, vk::SurfaceKHR& surface, const bool debug);

  GLFWwindow* get_glfw_window();

private:
  GLFWwindow* glfw_window_;
  bool debug_;
};

