#include "CubeMap.h"

namespace Ajisai {
	void CubeMap::CreateCubeMap(Device* device, const std::string filePath) {
		// load cube map from file

		std::vector<std::string> faces{
		"Models/skybox/4.png",
		"Models/skybox/5.png",
		"Models/skybox/2.png",
		"Models/skybox/3.png",
		"Models/skybox/0.png",
		"Models/skybox/1.png" };
		int texWidth, texHeight, numChannels;
		stbi_uc *img = stbi_load(faces[0].c_str(), &texWidth, &texHeight, &numChannels, STBI_rgb_alpha);
		stbi_image_free(img);
		VkDeviceSize imageSize = texWidth * texHeight * 4 * 6;
		VkDeviceSize picSize = texWidth * texHeight * 4;

		Buffer stagingBuffer;
		stagingBuffer.CreateBuffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);


		for (unsigned int i = 0; i < faces.size(); i++) {
			stbi_uc *pixels = stbi_load(faces[i].c_str(), &texWidth, &texHeight, &numChannels, 0);
			if (pixels) {
				void *data;
				if (vkMapMemory(device->mLogicalDevice, stagingBuffer.mBufferMemory, i*picSize, picSize, 0, &data) != VK_SUCCESS) {
					std::cout << "memory map fail: " << i << std::endl;
				}		
				memcpy(data, pixels, static_cast<size_t>(picSize));
				vkUnmapMemory(device->mLogicalDevice, stagingBuffer.mBufferMemory);
				stbi_image_free(pixels);
			}
			else {
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(pixels);
			}
		}

		// create image
		VkImageCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		createInfo.imageType = VK_IMAGE_TYPE_2D;
		createInfo.extent.width = static_cast<uint32_t>(texWidth);
		createInfo.extent.height = static_cast<uint32_t>(texHeight);
		createInfo.extent.depth = 1;
		createInfo.mipLevels = 1;
		createInfo.arrayLayers = 6;
		createInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		createInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		// for cube map image
		createInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

		if (vkCreateImage(device->mLogicalDevice, &createInfo, nullptr, &mImage) != VK_SUCCESS) {
			throw std::runtime_error("failed to create cubemap image");
		}

		
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device->mLogicalDevice, mImage, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(device->mPhysicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((memRequirements.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags &  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
				allocInfo.memoryTypeIndex = i;
				break;
			}
		}

		if (vkAllocateMemory(device->mLogicalDevice, &allocInfo, nullptr, &mImageMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate cubemap image memory");
		}

		vkBindImageMemory(device->mLogicalDevice, mImage, mImageMemory, 0);

		// transition from LAYOUT_UNDEFINED to LAYOUT_DST_OPTIMAL
		VkCommandBuffer commandBuffer;
		device->BeginSingleTimeCommands(commandBuffer);

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = mImage;

		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 6;

		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

		vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		device->EndSingleTimeCommands(commandBuffer);
		//device->CmdTransitionImageLayout(mImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		// in the middle, copy images from stagingbuffer to cubemap image
		device->BeginSingleTimeCommands(commandBuffer);
		std::array<VkBufferImageCopy, 6> copyRegion = {};
		for (int i = 0; i < 6; i++) {
			copyRegion[i].imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion[i].imageSubresource.baseArrayLayer = i;
			copyRegion[i].imageSubresource.layerCount = 1;
			copyRegion[i].imageSubresource.mipLevel = 0;

			copyRegion[i].imageOffset = { 0,0,0 };
			copyRegion[i].imageExtent = { (uint32_t)(texWidth), (uint32_t)(texHeight), 1};

			copyRegion[i].bufferOffset = texWidth * texHeight * 4 * i;
		}
		vkCmdCopyBufferToImage(commandBuffer, stagingBuffer.mBuffer, mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(copyRegion.size()), copyRegion.data());
		device->EndSingleTimeCommands(commandBuffer);

		// layout transfer from TRANSFER_DST
		device->BeginSingleTimeCommands(commandBuffer);

		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = mImage;

		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 6;

		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

		vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		device->EndSingleTimeCommands(commandBuffer);

		// create image view
		VkImageViewCreateInfo view = {};
		view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		view.format = VK_FORMAT_R8G8B8A8_UNORM;
		view.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
		view.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0,1,0,1 };
		view.subresourceRange.layerCount = 6;
		view.subresourceRange.levelCount = 1;
		view.image = mImage;

		if (vkCreateImageView(device->mLogicalDevice, &view, nullptr, &mImageView) != VK_SUCCESS) {
			std::cout << "fail to create cubemap image view";
		}

		// create sampler
		VkSamplerCreateInfo sampler = {};
		sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler.magFilter = VK_FILTER_LINEAR;
		sampler.minFilter = VK_FILTER_LINEAR;
		sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sampler.addressModeV = sampler.addressModeU;
		sampler.addressModeW = sampler.addressModeU;
		sampler.mipLodBias = 0.0f;
		sampler.compareOp = VK_COMPARE_OP_NEVER;
		sampler.minLod = 0.0f;
		sampler.maxLod = 0.0f;
		sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		sampler.anisotropyEnable = VK_TRUE;
		sampler.maxAnisotropy = 16;
		if (vkCreateSampler (device->mLogicalDevice, &sampler, nullptr, &mSampler) != VK_SUCCESS) {
			std::cout << "fail to create cubemap sampler";
		}

		// clean up staging buffer
		stagingBuffer.CleanUp(device);
	}

	void CubeMap::CleanUp(Device *device) {
		vkDestroySampler(device->mLogicalDevice, mSampler, nullptr);
		vkDestroyImageView(device->mLogicalDevice, mImageView, nullptr);
		vkDestroyImage(device->mLogicalDevice, mImage, nullptr);
		vkFreeMemory(device->mLogicalDevice, mImageMemory, nullptr);
	}
}


