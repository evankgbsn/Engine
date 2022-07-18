#ifndef VULKAN_PHYSICAL_DEVICE
#define VULKAN_PHYSICAL_DEVICE

#include <vector>
#include <optional>

#include <vulkan/vulkan.h>

class VulkanPhysicalDevice
{

public:

	struct QueueFamilies
	{
		std::optional<uint32_t> presentationIndex;
		std::optional<uint32_t> graphicsIndex;
		std::optional<uint32_t> computeIndex;
		std::optional<uint32_t> transferIndex;
		std::optional<uint32_t> sparseBindingIndex;
		std::optional<uint32_t> protectedIndex;
	};

public:

	VulkanPhysicalDevice() = delete;

	VulkanPhysicalDevice(VkPhysicalDevice&& device, VkPhysicalDeviceProperties&& properties, VkPhysicalDeviceFeatures&& features);

	~VulkanPhysicalDevice();

	VulkanPhysicalDevice(const VulkanPhysicalDevice&) = delete;

	VulkanPhysicalDevice& operator=(const VulkanPhysicalDevice&) = delete;

	VulkanPhysicalDevice(const VulkanPhysicalDevice&&) = delete;

	VulkanPhysicalDevice& operator=(const VulkanPhysicalDevice&&) = delete;

	const VkPhysicalDevice& operator()() const { return physicalDevice; };

	const VkPhysicalDeviceProperties& GetProperties() const { return properties; };

	void GetDisplayProperties();

	bool FindPresentationQueueFamily(const VkSurfaceKHR& surface);

	bool IsDeviceSuitable();

	void FinalizeDevice();

	bool SupportsSwapChain();

	const char* const GetName() const { return properties.deviceName; };

	const QueueFamilies& GetQueueFamilies() const { return queueFamilies; };

	VkDevice& GetLogicalDevice() { return logicalDevice; };

	int GetGraphicsQueueFamilyIndex() const { return queueFamilies.graphicsIndex.value(); };

	VkQueue& GetGraphicsQueue() { return graphicsQueue; };

	VkQueue& GetPresentationQueue() { return presentationQueue; };

private:

	void FindQueueFamilies();

	void CreateLogicalDevice();

	// The physical device handle.
	const VkPhysicalDevice physicalDevice;

	// The physical device properties.
	const VkPhysicalDeviceProperties properties;

	// The physical device features.
	const VkPhysicalDeviceFeatures features;

	// The queue families.
	QueueFamilies queueFamilies;

	// The logical device handle.
	VkDevice logicalDevice = VK_NULL_HANDLE;

	// The handle to the graphics queue.
	VkQueue graphicsQueue = VK_NULL_HANDLE;

	// The queue family properties.
	std::vector<VkQueueFamilyProperties> queueFamilyProperties;

	// The queue used for presentation.
	VkQueue presentationQueue = VK_NULL_HANDLE;

	// If the device supports VK_KHR_swapchain extension.
	bool supportsSwapchain = false;
};

#endif // VULKAN_PHYSICAL_DEVICE