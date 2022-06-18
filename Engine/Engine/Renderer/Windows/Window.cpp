#include "Window.h"

#include <unordered_set>
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

	bool foundDesiredFormat = false;
	for (const auto& format : surfaceInfo.surfaceFormats)
	{
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			surfaceFormat = format;
			Logger::Log(std::string("Using VK_FORMAT_B8G8R8A8_SRGB format with VK_COLOR_SPACE_SRGB_NONLINEAR_KHR color space."), Logger::Category::Success);
			foundDesiredFormat = true;
			break;
		}
	}

	if (!foundDesiredFormat)
	{
		surfaceFormat = surfaceInfo.surfaceFormats.front();
		Logger::Log(std::string("Defaulting to first found surface format because desired format was not found."), Logger::Category::Warning);
	}

	std::unordered_set<VkPresentModeKHR> modes(surfaceInfo.presentModes.begin(), surfaceInfo.presentModes.end());

	// Print all of the available present modes.
	for (const auto& mode : modes)
	{
		switch (mode)
		{
		case VK_PRESENT_MODE_MAILBOX_KHR:
			Logger::Log(std::string("Present Mode Mailbox available"));
			break;
		case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
			Logger::Log(std::string("Present Mode FIFO Relaxed available"));
			break;
		case VK_PRESENT_MODE_IMMEDIATE_KHR:
			Logger::Log(std::string("Present Mode Immediate available"));
			break;
		case VK_PRESENT_MODE_FIFO_KHR:
			Logger::Log(std::string("Present Mode FIFO available"));
			break;
		default:
			Logger::Log(std::string("Unknown Present Mode available"));
			break;
		}
	}
	
	// Choose the desired present mode.
	if (modes.find(VK_PRESENT_MODE_MAILBOX_KHR) != modes.end())
	{
		presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		Logger::Log(std::string("Present Mode set to Mailbox"), Logger::Category::Success);
	}
	else if (modes.find(VK_PRESENT_MODE_FIFO_RELAXED_KHR) != modes.end())
	{
		presentMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
		Logger::Log(std::string("Present Mode set to FIFO Relaxed"), Logger::Category::Warning);
	}
	else
	{
		presentMode = VK_PRESENT_MODE_FIFO_KHR;
		Logger::Log(std::string("Present Mode set to FIFO"), Logger::Category::Warning);
	}

	return surfaceInfo;
}
