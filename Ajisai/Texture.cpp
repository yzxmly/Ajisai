#include "Texture.h"

namespace Ajisai {
	void Texture::CreateTexture(Device* device, const std::string filePath) {
		int texWidth, texHeight, texChannels;

		stbi_uc *pixels = stbi_load(filePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

		if (!pixels) {
			throw std::runtime_error("failed to load texture file");
		}
		
		VkDeviceSize imageSize = texWidth * texHeight * 4;
		
		Buffer stagingBuffer;
		stagingBuffer.CreateBuffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void *data;
		vkMapMemory(device->mLogicalDevice, stagingBuffer.mBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(device->mLogicalDevice, stagingBuffer.mBufferMemory);

		stbi_image_free(pixels);

		mTexImage.CreateImage(device, texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		device->CmdTransitionImageLayout(mTexImage.mImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		device->CmdCopyBufferToImage(stagingBuffer.mBuffer, mTexImage.mImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
		device->CmdTransitionImageLayout(mTexImage.mImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		stagingBuffer.CleanUp(device);

		mTexImage.CreateImageView(device, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

		VkSamplerCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		createInfo.magFilter = VK_FILTER_LINEAR;
		createInfo.minFilter = VK_FILTER_LINEAR;
		createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.anisotropyEnable = VK_TRUE;
		createInfo.maxAnisotropy = 16;
		createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		createInfo.unnormalizedCoordinates = VK_FALSE;
		createInfo.compareEnable = VK_FALSE;
		createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		createInfo.mipLodBias = 0.0f;
		createInfo.minLod = 0.0f;
		createInfo.maxLod = 0.0f;

		if (vkCreateSampler(device->mLogicalDevice, &createInfo, nullptr, &mSampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create sampler");
		}
	}

	void Texture::CleanUp(Device* device) {
		vkDestroySampler(device->mLogicalDevice, mSampler, nullptr);
		mTexImage.CleanUp(device);
	}
}