#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <string>
#include <optional>

#include <vulkan/vulkan.h>

class VulkanPhysicalDevice;
class Window;

class Renderer
{

public:

	// Initialize the renderer.
	static void Initialize();

	// Terminate the renderer.
	static void Terminate();

	// Update the renderer.
	static bool Update();

	// Get the Vulkan instance.
	static const VkInstance& GetVulkanInstance();

	// Choose a device to be used for rendering.
	static void ChooseDevice(Window& window);

	// Get the chosen VulkanPhysicalDevice.
	static VulkanPhysicalDevice* GetVulkanPhysicalDevice();

private:

	Renderer();

	~Renderer();

	Renderer(const Renderer&) = delete;

	Renderer& operator=(const Renderer&) = delete;

	Renderer(const Renderer&&) = delete;

	Renderer& operator=(const Renderer&&) = delete;

	const std::vector<std::string>& GetSupportedInstanceExtensions();

	const std::vector<std::string>& GetSupportedInstanceLayers();

	void InitializeVulkan();

	void CreateVulkanInstance();

	void CreateVulkanDebugMessenger();

	void DestroyVulkanDebugMessenger();

	void CreateVulkanGraphicsPipeline();

	void TerminateVulkan();

	// Find all of the devices that support rendering on this machine.
	void FindVulkanPhysicalDevices();

	// Log all the info about the device.
	void LogVulkanPhysicalDeviceProperties(const VkPhysicalDeviceProperties& properties, std::string& vulkanPhysicalDeviceLog);
	void LogVulkanPhysicalDeviceFeatures(const VkPhysicalDeviceFeatures& features, std::string& vulkanPhysicalDeviceLog);

	// The debug callback for vulkan validation.
	static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

	// The single instance of the Renderer class.
	static Renderer* instance;

	// The Vulkan instance.
	VkInstance vulkanInstance = VK_NULL_HANDLE;

	// Information about this application that is passed to vulkan.
	VkApplicationInfo applicationInfo = {};

#ifndef NDEBUG
	// Used in debug to output vulkan messages.
	VkDebugUtilsMessengerEXT vulkanDebugMessenger = VK_NULL_HANDLE;
#endif // NDEBUG

	// All of the vulkan enabled physical devices.
	std::vector<VulkanPhysicalDevice*> vulkanPhysicalDevices;

	// The physical device chosen for rendering.
	VulkanPhysicalDevice* chosenDevice;

};

#endif //RENDERER_H