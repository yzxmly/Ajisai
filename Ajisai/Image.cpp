#include "Image.h"

namespace Ajisai {
	void Image::CreateImage(Device *device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
		VkImageCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		createInfo.imageType = VK_IMAGE_TYPE_2D;
		createInfo.extent.width = static_cast<uint32_t>(width);
		createInfo.extent.height = static_cast<uint32_t>(height);
		createInfo.extent.depth = 1;
		createInfo.mipLevels = 1;
		createInfo.arrayLayers = 1;
		createInfo.format = format;
		createInfo.tiling = tiling;
		createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		createInfo.usage = usage;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		createInfo.flags = 0;

		if (vkCreateImage(device->mLogicalDevice, &createInfo, nullptr, &mImage) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device->mLogicalDevice, mImage, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(device, memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device->mLogicalDevice, &allocInfo, nullptr, &mImageMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate texture image memory");
		}

		vkBindImageMemory(device->mLogicalDevice, mImage, mImageMemory, 0);
	}

	void Image::CreateImageView(Device *device, VkFormat format, VkImageAspectFlags aspectFlags) {
		mFormat = format;

		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = mImage;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = format;

		createInfo.subresourceRange.aspectMask = aspectFlags;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device->mLogicalDevice, &createInfo, nullptr, &mImageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}
	}

	uint32_t Image::FindMemoryType(Device *device, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(device->mPhysicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	void Image::CleanUp(Device *device) {
		vkDestroyImageView(device->mLogicalDevice, mImageView, nullptr);
		vkDestroyImage(device->mLogicalDevice, mImage, nullptr);
		vkFreeMemory(device->mLogicalDevice, mImageMemory, nullptr);
	}
}

