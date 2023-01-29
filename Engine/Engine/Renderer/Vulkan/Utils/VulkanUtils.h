#ifndef VULKANUTILS_H
#define VULKANUTILS_H

#include "../../../Utils/Logger.h"
#include <vulkan/vulkan.h>

#include <stdexcept>

namespace VulkanUtils
{
	void CheckResult(const VkResult& result, bool shouldThrow, bool shouldLog, const char* logMessage = "", Logger::Category category = Logger::Category::Error);
};

#endif // VULKANUTILS_H