#pragma once
#include "Headers.h"
namespace VkUtils {
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
}// namespace VkUtils