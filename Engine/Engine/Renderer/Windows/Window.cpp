#include "Window.h"

#include <stdexcept>

#include <GLFW/glfw3.h>

#include "../../Utils/Logger.h"
#include "../Renderer.h"
#include "../Vulkan/VulkanPhysicalDevice.h"

Window::Window(uint32_t w, uint32_t h, std::string&& windowName) :
	name(windowName),
	width(w),
	height(h)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);

	if (!window)
	{
		Logger::Log(std::string("Could not create GLFW window"), Logger::Category::Error);
		throw std::runtime_error("Could not create GLFW window");
	}

	// Create the Vulkan Surface.
	VkResult result = glfwCreateWindowSurface(Renderer::GetVulkanInstance(), window, nullptr, &surface);
	if (result != VK_SUCCESS)
	{
		std::string errorStr = std::string("Could not create a surface for window ") + name + std::string(" Window::Window");
		Logger::Log(std::string(errorStr), Logger::Category::Error);
		throw std::runtime_error(errorStr.c_str());
	}

	Renderer::ChooseDevice(*this);
}

Window::~Window()
{
	vkDestroySurfaceKHR(Renderer::GetVulkanInstance(), surface, nullptr);
}

bool Window::Update()
{
	if (glfwWindowShouldClose(window))
	{
		return false;
	}

	glfwPollEvents();

	return true;
}

const Window::SurfaceInfo& Window::GetSurfaceInfo(const VulkanPhysicalDevice& device)
{
	VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device(), surface, &surfaceInfo.surfaceInfo);

	if (result != VK_SUCCESS)
	{
		Logger::Log(std::string("Could not get surface info for window ") + name + std::string(" using device ") + std::string(device.GetProperties().deviceName), Logger::Category::Error);
	}

	uint32_t formatCount = 0;
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(device(), surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		surfaceInfo.surfaceFormats.resize(formatCount);
		result = vkGetPhysicalDeviceSurfaceFormatsKHR(device(), surface, &formatCount, surfaceInfo.surfaceFormats.data());
	}

	if (result != VK_SUCCESS)
	{
		Logger::Log(std::string("Could not get surface formats for window ") + name + std::string(" using device ") + std::string(device.GetProperties().deviceName), Logger::Category::Error);
	}

	uint32_t presentModeCount = 0;
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(device(), surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		surfaceInfo.presentModes.resize(presentModeCount);
		result = vkGetPhysicalDeviceSurfacePresentModesKHR(device(), surface, &presentModeCount, surfaceInfo.presentModes.data());
	}

	if (result != VK_SUCCESS)
	{
		Logger::Log(std::string("Could not get surface present modes for window ") + name + std::string(" using device ") + std::string(device.GetProperties().deviceName), Logger::Category::Error);
	}

	return surfaceInfo;
}
