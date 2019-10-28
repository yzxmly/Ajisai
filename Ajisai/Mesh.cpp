#include "Mesh.h"

namespace Ajisai {
	void Mesh::CreateBuffers(Device *device) {
		VkDeviceSize vetexBufferSize = sizeof(Vertex) * mVertices.size();
	
		Buffer stagingBuffer;
		
		stagingBuffer.CreateBuffer(device, vetexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* vertexData;
		vkMapMemory(device->mLogicalDevice, stagingBuffer.mBufferMemory, 0, vetexBufferSize, 0, &vertexData);
		memcpy(vertexData, mVertices.data(), vetexBufferSize);
		vkUnmapMemory(device->mLogicalDevice, stagingBuffer.mBufferMemory);

		mVertexBuffer.CreateBuffer(device, vetexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		device->CmdCopyBuffer(stagingBuffer.mBuffer, mVertexBuffer.mBuffer, vetexBufferSize);

		stagingBuffer.CleanUp(device);

		VkDeviceSize indexBufferSize = sizeof(mIndices[0]) * mIndices.size();

		stagingBuffer.CreateBuffer(device, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* indexData;
		vkMapMemory(device->mLogicalDevice, stagingBuffer.mBufferMemory, 0, indexBufferSize, 0, &indexData);
		memcpy(indexData, mIndices.data(), indexBufferSize);
		vkUnmapMemory(device->mLogicalDevice, stagingBuffer.mBufferMemory);

		mIndexBuffer.CreateBuffer(device, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		device->CmdCopyBuffer(stagingBuffer.mBuffer,mIndexBuffer.mBuffer, indexBufferSize);

		stagingBuffer.CleanUp(device);
	}
	void Mesh::CleanUp(Device *device) {
		mVertexBuffer.CleanUp(device);
		mIndexBuffer.CleanUp(device);
	}
}