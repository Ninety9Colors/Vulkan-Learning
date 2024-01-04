#pragma once
#include "Headers.h"
#include "VkUtils/SwapchainDetails.h"
#include "VkUtils/QueueFamilies.h"

namespace VkInit {

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

  VkUtils::SwapChainBundle create_swapchain(const vk::PhysicalDevice& physical_device, const vk::Device& logical_device, const vk::SurfaceKHR& surface, GLFWwindow* window, const bool debug) {
    if (debug) {
      std::cout << "Creating Swapchain..." << std::endl;
    }

    VkUtils::SwapChainBundle bundle;

    VkUtils::SwapChainSupportDetails details = VkUtils::query_swapchain_support(physical_device, surface, debug);

    vk::SurfaceFormatKHR format = choose_swap_surface_format(details.Formats, debug);
    vk::PresentModeKHR present_mode = choose_present_mode(details.PresentModes, debug);
    vk::Extent2D extent = choose_swap_extent(details.Capabilities, window, debug);

    uint32_t image_count = (std::min)(details.Capabilities.minImageCount + 1,
      details.Capabilities.maxImageCount);

    vk::SwapchainCreateInfoKHR create_info{};

    create_info.flags = vk::SwapchainCreateFlagsKHR{};
    create_info.surface = surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = format.format;
    create_info.imageColorSpace = format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    VkUtils::QueueFamilyIndices indices = VkUtils::find_queue_families(physical_device, surface, debug);
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

}// namespace VkInit