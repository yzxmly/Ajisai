#include "Buffer.h"

namespace Ajisai {
	void Buffer::CreateBuffer(Device* device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = size;
		createInfo.usage = usage;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device->mLogicalDevice, &createInfo, nullptr, &mBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create buffer");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device->mLogicalDevice, mBuffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(device, memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device->mLogicalDevice, &allocInfo, nullptr, &mBufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate buffer memory");
		}

		vkBindBufferMemory(device->mLogicalDevice, mBuffer, mBufferMemory, 0);
	}

	uint32_t Buffer::FindMemoryType(Device* device, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(device->mPhysicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	void Buffer::CleanUp(Device* device) {
		vkDestroyBuffer(device->mLogicalDevice, mBuffer, nullptr);
		vkFreeMemory(device->mLogicalDevice, mBufferMemory, nullptr);
	}
}