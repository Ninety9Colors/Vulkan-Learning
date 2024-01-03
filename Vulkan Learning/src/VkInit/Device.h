#pragma once
#include "Headers.h"
namespace VkInit {
  struct QueueFamilyIndices {
    std::optional<uint32_t> GraphicsFamily;
    std::optional<uint32_t> PresentFamily;

    bool is_complete() {
      return GraphicsFamily.has_value() && PresentFamily.has_value();
    }
  };

  struct SwapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR Capabilities;
    std::vector<vk::SurfaceFormatKHR> Formats;
    std::vector<vk::PresentModeKHR> PresentModes;
  };

  struct SwapChainBundle {
    vk::SwapchainKHR Swapchain;
    vk::Format Format;
    vk::Extent2D Extent;
  };

  // only run in debug
  void log_physical_device_properties(const vk::PhysicalDevice& device) {
    vk::PhysicalDeviceProperties props = device.getProperties();

    std::cout << "Device Name: " << props.deviceName << std::endl;
    std::cout << "Device Type: ";

    switch (props.deviceType) {
    case vk::PhysicalDeviceType::eDiscreteGpu:{
      std::cout << "Discrete Gpu" << std::endl;
      break;
    }
    case vk::PhysicalDeviceType::eIntegratedGpu: {
      std::cout << "Integrated Gpu" << std::endl;
      break;
    }
    case vk::PhysicalDeviceType::eCpu: {
      std::cout << "Cpu" << std::endl;
      break;
    }
    case vk::PhysicalDeviceType::eVirtualGpu: {
      std::cout << "Virtual Cpu" << std::endl;
      break;
    }
    default: {
      std::cout << "Other" << std::endl;
      break;
    }
    }
  }

  bool device_is_supported(const vk::PhysicalDevice& device, const bool debug) {
    std::vector<const char*> extensions{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    if (debug) {
      std::cout << std::endl << "Requesting Device Extensions: ";
      for (const char* extension : extensions) {
        std::cout << extension << ", ";
      }
      std::cout << std::endl << std::endl;
    }

    std::vector<vk::ExtensionProperties> supported_extensions = device.enumerateDeviceExtensionProperties();

    if (debug) {
      std::cout << "Supported Device Extensions: ";
      for (vk::ExtensionProperties props : supported_extensions) {
        std::cout << props.extensionName << ", ";
      }
      std::cout << std::endl << std::endl;
    }

    for (const char* req : extensions) {
      bool temp_supported_layers = false;
      for (vk::ExtensionProperties prop : supported_extensions) {
        if (strcmp(req, prop.extensionName) == 0)
          temp_supported_layers = true;
      }
      if (temp_supported_layers) {
        if (debug)
          std::cout << req << " is supported!" << std::endl << std::endl;
      }
      else {
        if (debug)
          std::cout << req << " is not supported!" << std::endl << std::endl;
        return false;
      }
    }

    return true;
  }

  QueueFamilyIndices find_queue_families(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface, const bool debug) {
    if (debug) {
      std::cout << "Finding Queue Families..." << std::endl;
    }
    QueueFamilyIndices indices;

    std::vector<vk::QueueFamilyProperties> family_props{ device.getQueueFamilyProperties() };
    if (debug) {
      std::cout << "Device supports " << family_props.size() << " queue families!" << std::endl;
    }

    int indice{ 0 };
    for (const vk::QueueFamilyProperties& prop : family_props) {
      if (prop.queueFlags & vk::QueueFlagBits::eGraphics) {
        indices.GraphicsFamily = indice;
        if (debug) {
          std::cout << "Queue family " << indice << " is suitable for graphics!" << std::endl;
        }
      }

      if (device.getSurfaceSupportKHR(indice, surface)) {
        indices.PresentFamily = indice;
        if (debug) {
          std::cout << "Queue family " << indice << " is suitable for presenting!" << std::endl;
        }
      }

      if (indices.is_complete()) {
        break;
      }
      ++indice;
    }

    return indices;
  }

  SwapChainSupportDetails query_swapchain_support(const vk::PhysicalDevice& physical_device, const vk::SurfaceKHR& surface, const bool debug) {
    SwapChainSupportDetails details;

    if (debug) {
      std::cout << "Querying Swapchain Support..." << std::endl;
    }

    details.Capabilities = physical_device.getSurfaceCapabilitiesKHR(surface);

    details.Formats = physical_device.getSurfaceFormatsKHR(surface);

    details.PresentModes = physical_device.getSurfacePresentModesKHR(surface);

    return details;
  }

  vk::SurfaceFormatKHR choose_swap_surface_format(std::vector <vk::SurfaceFormatKHR> formats, const bool debug) {
    if (debug) {
      std::cout << "Selecting Surface Format..." << std::endl;
    }
    for (const auto& format : formats) {
      if (format.format == vk::Format::eB8G8R8A8Snorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
        return format;
      }
    }

    return formats[0];
  }

  vk::PresentModeKHR choose_present_mode(std::vector<vk::PresentModeKHR> modes, const bool debug) {
    if (debug) {
      std::cout << "Selecting Present Mode..." << std::endl;
    }

    for (const auto& mode : modes) {
      if (mode == vk::PresentModeKHR::eMailbox) {
        return mode;
      }
    }

    return vk::PresentModeKHR::eFifo;
  }

  vk::Extent2D choose_swap_extent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window, const bool debug) {
    if (debug) {
      std::cout << "Selecting Surface Extent..." << std::endl;
    }
    if (capabilities.currentExtent.width != UINT32_MAX) {
      return capabilities.currentExtent;
    }
    else {
      int width, height;
      glfwGetFramebufferSize(window, &width, &height);

      VkExtent2D actualExtent = {
          static_cast<uint32_t>(width),
          static_cast<uint32_t>(height)
      };

      actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
      actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

      return actualExtent;
    }
  }

  SwapChainBundle create_swapchain(const vk::PhysicalDevice& physical_device, const vk::Device& logical_device, const vk::SurfaceKHR& surface, GLFWwindow* window, const bool debug) {
    if (debug) {
      std::cout << "Creating Swapchain..." << std::endl;
    }

    SwapChainBundle bundle;
    
    SwapChainSupportDetails details = query_swapchain_support(physical_device, surface, debug);

    vk::SurfaceFormatKHR format = choose_swap_surface_format(details.Formats, debug);
    vk::PresentModeKHR present_mode = choose_present_mode(details.PresentModes, debug);
    vk::Extent2D extent = choose_swap_extent(details.Capabilities, window, debug);

    uint32_t image_count = (std::min) (details.Capabilities.minImageCount + 1, 
                                    details.Capabilities.maxImageCount);

    vk::SwapchainCreateInfoKHR create_info {};

    create_info.flags = vk::SwapchainCreateFlagsKHR{};
    create_info.surface = surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = format.format;
    create_info.imageColorSpace = format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    QueueFamilyIndices indices = find_queue_families(physical_device, surface, debug);
    uint32_t queue_family_indices[] = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };

    if (queue_family_indices[0] != queue_family_indices[1]) {
      create_info.imageSharingMode = vk::SharingMode::eConcurrent;
      create_info.queueFamilyIndexCount = 2;
      create_info.pQueueFamilyIndices = queue_family_indices;
    }
    else {
      create_info.imageSharingMode = vk::SharingMode::eExclusive;
    }

    create_info.preTransform = details.Capabilities.currentTransform;
    create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    create_info.presentMode = present_mode;
    create_info.clipped = true;
    create_info.oldSwapchain = vk::SwapchainKHR{ nullptr };

    vk::SwapchainKHR swapchain{};
    try {
      bundle.Swapchain = logical_device.createSwapchainKHR(create_info);
      bundle.Format = format.format;
      bundle.Extent = extent;
      if (debug) {
        std::cout << "Swapchain Successfully Created!" << std::endl << std::endl;
      }
    }
    catch (vk::SystemError e) {
      throw std::runtime_error("Failed to create swapchain!");
    }

    return bundle;
  }

  vk::PhysicalDevice choose_physical_device(const vk::Instance& instance, const vk::SurfaceKHR& surface, const bool debug) {
    if (debug) {
      std::cout << "Choosing Physical Device..." << std::endl;
    }

    std::vector<vk::PhysicalDevice> devices_vector{ instance.enumeratePhysicalDevices() };

    if (debug) {
      std::cout << std::endl << "Physical Devices: " << std::endl << std::endl;
    }
    for (vk::PhysicalDevice dev : devices_vector) {
      if (debug) {
        log_physical_device_properties(dev);
      }
      if (device_is_supported(dev, debug)) {
        SwapChainSupportDetails swapchain_support = query_swapchain_support(dev, surface, debug);
        if (!swapchain_support.Formats.empty() && !swapchain_support.PresentModes.empty()) {
          return dev;
        }
      }
    }

    return nullptr;
  }

  vk::Device create_logical_device(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface, const bool debug) {
    if (debug) {
      std::cout << "Creating Logical Device..." << std::endl;
    }
    QueueFamilyIndices indices = find_queue_families(device, surface, debug);
    std::vector<uint32_t> unique_indices;
    unique_indices.push_back(indices.GraphicsFamily.value());
    if (indices.GraphicsFamily.value() != indices.PresentFamily.value()) {
      unique_indices.push_back(indices.PresentFamily.value());
    }

    float queue_priority{ 1.0f };
    std::vector<vk::DeviceQueueCreateInfo> queue_infos;
    for (uint32_t unique_indice : unique_indices) {
      vk::DeviceQueueCreateInfo queue_info {
      vk::DeviceQueueCreateFlags(),
      unique_indice,
      1,
      &queue_priority
      };

      queue_infos.push_back(queue_info);
    }

    vk::PhysicalDeviceFeatures device_features {};

    std::vector<const char*> layers{};
    if (debug) {
      layers.push_back("VK_LAYER_KHRONOS_validation");
    }

    std::vector<const char*> device_extensions{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    vk::DeviceCreateInfo create_info {
      vk::DeviceCreateFlags{},
      static_cast<uint32_t>(queue_infos.size()), queue_infos.data(),
      static_cast<uint32_t>(layers.size()), layers.data(),
      static_cast<uint32_t>(device_extensions.size()), device_extensions.data(),
      &device_features
    };

    try {
      vk::Device logical_device = device.createDevice(create_info);
      if (debug) {
        std::cout << "Device successfully abstracted!" << std::endl << std::endl;
      }
      return logical_device;
    }
    catch (vk::SystemError e) {
      if (debug) {
        std::cout << "Device creation failed!" << std::endl;
      }
      return nullptr;
    }
    
    return nullptr;
  }

  std::array<vk::Queue, 2> get_queue(const vk::PhysicalDevice & physical_device, const vk::Device& device, const vk::SurfaceKHR& surface, const bool debug) {
    if (debug) {
      std::cout << "Retrieving Graphics Queue..." << std::endl;
    }
    QueueFamilyIndices indices = find_queue_families(physical_device, surface, debug);

    return {
      device.getQueue(indices.GraphicsFamily.value(), 0),
      device.getQueue(indices.PresentFamily.value(), 0)
    };
  }
} // namespace VkInit