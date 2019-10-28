#pragma once
#include "Core.h"
#include "Device.h"
#include "Buffer.h"

namespace Ajisai {
	struct Vertex {
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 texCoord;
		glm::vec3 tangent;
		glm::vec3 bitangent;

		static VkVertexInputBindingDescription GetBindingDescription() {
			VkVertexInputBindingDescription description = {};
			description.binding = 0;
			description.stride = sizeof(Vertex);
			description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return description;
		}

		static std::array<VkVertexInputAttributeDescription, 5> GetAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 5> descriptions = {};
			descriptions[0].binding = 0;
			descriptions[0].location = 0;
			descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			descriptions[0].offset = offsetof(Vertex, pos);

			descriptions[1].binding = 0;
			descriptions[1].location = 1;
			descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			descriptions[1].offset = offsetof(Vertex, normal);

			descriptions[2].binding = 0;
			descriptions[2].location = 2;
			descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			descriptions[2].offset = offsetof(Vertex, texCoord);

			descriptions[3].binding = 0;
			descriptions[3].location = 3;
			descriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
			descriptions[3].offset = offsetof(Vertex, tangent);

			descriptions[4].binding = 0;
			descriptions[4].location = 4;
			descriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
			descriptions[4].offset = offsetof(Vertex, bitangent);

			return descriptions;
		}
	};

	class Mesh
	{
	public:
		std::vector<Vertex> mVertices;
		std::vector<uint16_t> mIndices;

		std::vector<int> mTextureIdx;
		std::vector<std::string> mTextureType;

		Buffer mVertexBuffer, mIndexBuffer;

		//std::vector<unsigned int> mTextureIndices;

		void CreateBuffers(Device *device);
		void CleanUp(Device *device);
	};

}

