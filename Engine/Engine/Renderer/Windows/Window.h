#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

struct GLFWwindow;
class GraphicsPipeline;
class VulkanPhysicalDevice;

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

	const VkSurfaceKHR& GetSurface() const { return surface; };

	const VkViewport& GetViewport() const { return viewport; };

	const VkRect2D& GetScissor() const { return scissor; };

	const VkSurfaceFormatKHR& GetSurfaceFormat() const { return surfaceFormat; };

	// The struct of swapchain info.
	struct SurfaceInfo
	{
		VkSurfaceCapabilitiesKHR surfaceInfo{};
		std::vector<VkSurfaceFormatKHR> surfaceFormats = std::vector<VkSurfaceFormatKHR>();
		std::vector<VkPresentModeKHR> presentModes = std::vector<VkPresentModeKHR>();
	};

	// This must be called before CreateSwapchain.
	const SurfaceInfo& GetSurfaceInfo(const VulkanPhysicalDevice& device);

	// GetSurfaceInfo needs to be called before CreateSwapchain.
	void CreateSwapchain();

	void CreateFramebuffers();

private:

	void Draw();

	void RecordCommands(int imageIndex);

	void CreateSyncObjects();

	// The GLFW window handle.
	GLFWwindow* window = nullptr;

	// The vulkan surface for this window.
	VkSurfaceKHR surface = VK_NULL_HANDLE;

	// The vulkan swap chain for this surface.
	VkSwapchainKHR swapchain = VK_NULL_HANDLE;

	// The chosen present mode to use for the swapchain.
	VkPresentModeKHR presentMode{};

	// The surface format to use for the swapchain.
	VkSurfaceFormatKHR surfaceFormat{};

	// The extent of the swapchain. This is in pixels.
	VkExtent2D swapchainExtent{};

	// The info about the surface and the device using the surface.
	SurfaceInfo surfaceInfo{};

	// The swapchain images.
	std::vector<VkImage> swapchainImages;

	// The swapchain image views that we can render to.
	std::vector<VkImageView> swapchainImageViews;

	// The frame buffers for the swap chain images.
	std::vector<VkFramebuffer> framebuffers;

	// A viewport basically describes the region of the framebuffer that the output will be rendered to.This will almost always be (0, 0) to (width, height)
	VkViewport viewport;

	// The scissor.
	VkRect2D scissor{};

	std::string name = "";
	uint32_t width = 0;
	uint32_t height = 0;

	GraphicsPipeline* graphicsPipeline;

	// Syncronization objects.
	VkSemaphore imageAvailable = VK_NULL_HANDLE;
	VkSemaphore renderFinished = VK_NULL_HANDLE;
	VkFence inFlight = VK_NULL_HANDLE;
};

#endif // WINDOW_H