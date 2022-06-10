#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

class VulkanPhysicalDevice;
struct GLFWwindow;

class Window
{

public:

	Window() = delete;

	Window(uint32_t width, uint32_t height, std::string&& name);

	~Window();

	Window(const Window&) = delete;

	Window& operator=(const Window&) = delete;

	Window(const Window&&) = delete;

	Window& operator=(const Window&&) = delete;

	bool Update();

	const std::string& GetName() const { return name; };

	const uint32_t& GetWidth() const { return width; };

	const uint32_t& GetHeight() const { return height; };

	const VkSurfaceKHR& GetSurface() const { return surface; }

	// The struct of swapchain info.
	struct SurfaceInfo
	{
		VkSurfaceCapabilitiesKHR surfaceInfo = {};
		std::vector<VkSurfaceFormatKHR> surfaceFormats = std::vector<VkSurfaceFormatKHR>();
		std::vector<VkPresentModeKHR> presentModes = std::vector<VkPresentModeKHR>();
	};

	const SurfaceInfo& GetSurfaceInfo(const VulkanPhysicalDevice& device);

private:

	// The GLFW window handle.
	GLFWwindow* window = nullptr;

	// The vulkan surface for this window.
	VkSurfaceKHR surface = VK_NULL_HANDLE;

	// The info about the surface and the device using the surface.
	SurfaceInfo surfaceInfo = {};

	std::string name = "";
	uint32_t width = 0;
	uint32_t height = 0;
};

#endif // WINDOW_H