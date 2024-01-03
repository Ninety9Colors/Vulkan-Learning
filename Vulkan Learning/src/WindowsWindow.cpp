#include "WindowsWindow.h"

WindowsWindow::WindowsWindow(bool debug) {
  debug_ = debug;
  if (debug_) {
    std::cout << "Initializing GLFW..." << std::endl;
    if (!glfwInit())
    {
      std::cout << "GLFW Failed to initialize!" << std::endl;
    }
  } else {
    glfwInit();
  }

  glfwSetErrorCallback(error_callback);

  create_glfw_window();
  
}

WindowsWindow::~WindowsWindow() {
  if (debug_) {
    std::cout << "Deleting GLFW Window..." << std::endl;
  }
  glfwDestroyWindow(glfw_window_);
  glfwTerminate();
}

void WindowsWindow::error_callback(int error_code, const char* description) {
  std::cout << "GLFW error code " << error_code << ": " << description << std::endl;
}

void WindowsWindow::on_update() {
  glfwPollEvents();
}

void WindowsWindow::create_glfw_window() {
  if (debug_) {
    std::cout << "Initializing GLFW Window..." << std::endl;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  glfw_window_ = glfwCreateWindow(640, 480, "Vulkan Learning", nullptr, nullptr);

  if (debug_) {
    if (!glfw_window_)
    {
      std::cout << "GLFW Window failed to initialize!" << std::endl;
    }
  }
}

void WindowsWindow::create_surface(const vk::Instance& instance, vk::SurfaceKHR& surface, const bool debug) {
  if (debug) {
    std::cout << std::endl << "Creating Window Surface..." << std::endl;
  }

  vk::Win32SurfaceCreateInfoKHR surface_info {
    vk::Win32SurfaceCreateFlagsKHR{},
    GetModuleHandle(nullptr),
    glfwGetWin32Window(glfw_window_)
  };

  try {
    surface = instance.createWin32SurfaceKHR(surface_info);
    if (debug) {
      std::cout << "Window Surface Created!" << std::endl;
    }
  }
  catch (vk::SystemError e) {
    if (debug) {
      std::cout << "Window Surface Creation Failed!" << std::endl;
    }
  }
}

GLFWwindow* WindowsWindow::get_glfw_window() {
  return glfw_window_;
}
