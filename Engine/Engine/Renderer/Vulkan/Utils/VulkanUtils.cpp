#include "VulkanUtils.h"

void VulkanUtils::CheckResult(const VkResult& result, bool shouldThrow, bool shouldLog, const char* logMessage, Logger::Category category)
{
	if (result != VK_SUCCESS)
	{
		if (shouldLog)
		{
			Logger::Log(std::string(logMessage), category);
		}

		if (shouldThrow)
		{
			throw std::runtime_error(logMessage);
		}
	}
}