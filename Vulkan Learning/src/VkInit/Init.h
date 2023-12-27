#pragma once
#include "glfw3.h"

#include "Headers.h"

namespace VkInit {
  bool supported(std::vector<const char*> extensions, std::vector<const char*> layers, const bool debug) {

    // layers
    uint32_t supported_layers;
    vk::enumerateInstanceLayerProperties(&supported_layers, nullptr);

    std::vector<vk::LayerProperties> layer_properties { supported_layers };
    vk::enumerateInstanceLayerProperties(&supported_layers, layer_properties.data());

    if (debug) {
      std::cout << std::endl << "Supported Layers: ";
      for (vk::LayerProperties prop : layer_properties) {
        std::cout << prop.layerName << ", ";
      }
      std::cout << std::endl << std::endl;
    }

    for (const char* layer : layers) {
      bool temp_supported_layers = false;
      for (vk::LayerProperties prop : layer_properties) {
        if (strcmp(layer, prop.layerName) == 0)
          temp_supported_layers = true;
      }
      if (temp_supported_layers) {
        if (debug)
          std::cout << layer << " is supported!" << std::endl << std::endl;
      }
      else {
        if (debug)
          std::cout << layer << " is not supported!" << std::endl << std::endl;
        return false;
      }
    }

    // extensions

    uint32_t supported_extensions;
    vk::enumerateInstanceLayerProperties(&supported_extensions, nullptr);

    std::vector<vk::ExtensionProperties> extension_properties{ supported_extensions };
    vk::enumerateInstanceExtensionProperties(nullptr, &supported_extensions, extension_properties.data());

    if (debug) {
      std::cout << "Supported Extensions: ";
      for (vk::ExtensionProperties prop : extension_properties) {
        std::cout << prop.extensionName << ", ";
      }
      std::cout << std::endl << std::endl;
    }

    for (const char* extension : extensions) {
      bool temp_supported_extensions = false;
      for (vk::ExtensionProperties prop : extension_properties) {
        if (strcmp(extension, prop.extensionName) == 0)
          temp_supported_extensions = true;
      }
      if (temp_supported_extensions) {
        if (debug)
          std::cout << extension << " is supported!" << std::endl << std::endl;
        else {
          if (debug)
            std::cout << extension << " is not supported!" << std::endl << std::endl;
          return false;
        }
      }
    }

    return true;
  }

  vk::Instance make_instance(const char* application_name, bool debug) {

    uint32_t version;
    vkEnumerateInstanceVersion(&version);

    if (debug) {
      std::cout << "System supports vulkan version: " << VK_API_VERSION_VARIANT(version) << ", " <<
        VK_API_VERSION_MAJOR(version) << ", " << VK_API_VERSION_MINOR(version) << ", "
        << VK_API_VERSION_PATCH(version) << std::endl;
    }

    version &= ~(0xFFFU); // remove patch for compatibility

    uint32_t count;
    const char** extensions = glfwGetRequiredInstanceExtensions(&count);

    std::vector<const char*> extensions_vector { extensions, extensions+count };
    if (debug)
      extensions_vector.push_back("VK_EXT_debug_utils");

    std::vector<const char*> layers_vector{
      "VK_LAYER_KHRONOS_validation"
    };
    
    if (!supported(extensions_vector, layers_vector, debug))
      return nullptr;

    vk::ApplicationInfo app_info {
      application_name,
      version,
      "Engine",
      version,
      version
    };

    vk::InstanceCreateInfo create_info{
      vk::InstanceCreateFlags(),
      &app_info,
      static_cast<uint32_t>(layers_vector.size()), layers_vector.data(),
      static_cast<uint32_t>(extensions_vector.size()), extensions_vector.data()
    };

    try {
      if (debug) {
        std::cout << "Creating Instance..." << std::endl;
      }
      return vk::createInstance(create_info);
    }
    catch (vk::SystemError e) {
      if (debug) {
        std::cout << "Instance Creation Failed!" << std::endl;
      }
      return nullptr;
    }

  }

  VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback (
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
  }

  // should only run in debug mode
  vk::DebugUtilsMessengerEXT create_debug_messenger (vk::Instance& instance, vk::DispatchLoaderDynamic& dispatch_loader) {
    std::cout << "Creating Debug Messenger..." << std::endl;

    vk::DebugUtilsMessengerCreateInfoEXT debug_create_info{
      vk::DebugUtilsMessengerCreateFlagsEXT(),

      vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,

      vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
      vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
      vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
      vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding,

      debug_callback,
      nullptr
    };

    return instance.createDebugUtilsMessengerEXT(debug_create_info, nullptr, dispatch_loader);
  }

}// namespace VkInit