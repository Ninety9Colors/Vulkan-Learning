#pragma once
#include "glfw3.h"

#include "Headers.h"

class WindowsWindow {
public:
  WindowsWindow(bool debug);
  ~WindowsWindow();

  static void error_callback(int error, const char* description);
  void on_update();
  void create_glfw_window();

private:
  GLFWwindow* glfw_window_;
  bool debug_;
};

