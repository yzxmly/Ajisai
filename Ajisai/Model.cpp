#include "Model.h"
#include "stb_image.h"

namespace Ajisai {

	void Model::LoadObject(std::string filePath) {
		Assimp::Importer import;
		const aiScene *scene = import.ReadFile(filePath, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_PreTransformVertices | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "Assimp error: " << import.GetErrorString() << std::endl;
			return;
		}

		mDirectory = filePath.substr(0, filePath.find_last_of('\\'));
		std::cout << "Model Directory: " << mDirectory << std::endl;

		mMeshes.resize(scene->mNumMeshes);
		std::cout << "total mesh number: " << scene->mNumMeshes << std::endl;

		unsigned int textureSize = 0;
		for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
			auto mat = scene->mMaterials[i];

			InitializeTexture(mat, aiTextureType_DIFFUSE, "texture_diffuse");
			InitializeTexture(mat, aiTextureType_SPECULAR, "texture_specular");
			InitializeTexture(mat, aiTextureType_HEIGHT, "texture_normal");
		}

		mTextures.resize(mTexturePath.size());

		LoadTexture();

		//ProcessNode(scene->mRootNode, scene, mMeshes);

		for (unsigned int i = 0; i < scene->mRootNode->mNumChildren; ++i) {
			auto idx = scene->mRootNode->mChildren[i]->mMeshes[0];
			aiMesh *mesh = scene->mMeshes[idx];
			ProcessMesh(mesh, scene, mMeshes[i]);
		}

	}

	void Model::MakeScreen() {
		mMeshes.resize(1);
		mMeshes[0].mVertices.resize(4);
		mMeshes[0].mIndices.resize(6);

		mMeshes[0].mVertices[0].pos = glm::vec3(-1.0f, 1.0f, 0.0f);
		mMeshes[0].mVertices[1].pos = glm::vec3(-1.0f, -1.0f, 0.0f);
		mMeshes[0].mVertices[2].pos = glm::vec3(1.0f, 1.0f, 0.0f);
		mMeshes[0].mVertices[3].pos = glm::vec3(1.0f, -1.0f, 0.0f);

		mMeshes[0].mVertices[0].normal = glm::vec3(-1.0f, 1.0f, 0.0f);
		mMeshes[0].mVertices[1].normal = glm::vec3(-1.0f, -1.0f, 0.0f);
		mMeshes[0].mVertices[2].normal = glm::vec3(1.0f, 1.0f, 0.0f);
		mMeshes[0].mVertices[3].normal = glm::vec3(1.0f, -1.0f, 0.0f);

		mMeshes[0].mVertices[0].tangent = glm::vec3(-1.0f, 1.0f, 0.0f);
		mMeshes[0].mVertices[1].tangent = glm::vec3(-1.0f, -1.0f, 0.0f);
		mMeshes[0].mVertices[2].tangent = glm::vec3(1.0f, 1.0f, 0.0f);
		mMeshes[0].mVertices[3].tangent = glm::vec3(1.0f, -1.0f, 0.0f);

		mMeshes[0].mVertices[0].bitangent = glm::vec3(-1.0f, 1.0f, 0.0f);
		mMeshes[0].mVertices[1].bitangent = glm::vec3(-1.0f, -1.0f, 0.0f);
		mMeshes[0].mVertices[2].bitangent = glm::vec3(1.0f, 1.0f, 0.0f);
		mMeshes[0].mVertices[3].bitangent = glm::vec3(1.0f, -1.0f, 0.0f);

		mMeshes[0].mIndices[0] = 0;
		mMeshes[0].mIndices[1] = 2;
		mMeshes[0].mIndices[2] = 1;
		mMeshes[0].mIndices[3] = 1;
		mMeshes[0].mIndices[4] = 2;
		mMeshes[0].mIndices[5] = 3;

		mMeshes[0].CreateBuffers(mDevice);
	}

	void Model::MakeGround(float xWidth, float y, float zWidth) {
		mMeshes.resize(1);
		mMeshes[0].mVertices.resize(4);
		mMeshes[0].mIndices.resize(6);

		mMeshes[0].mVertices[0].pos = glm::vec3(-xWidth, y, -zWidth);
		mMeshes[0].mVertices[1].pos = glm::vec3(-xWidth, y, zWidth);
		mMeshes[0].mVertices[2].pos = glm::vec3(xWidth, y, -zWidth);
		mMeshes[0].mVertices[3].pos = glm::vec3(xWidth, y, zWidth);

		mMeshes[0].mVertices[0].normal = glm::vec3(0.0f, 1.0f, 0.0f);
		mMeshes[0].mVertices[1].normal = glm::vec3(0.0f, 1.0f, 0.0f);
		mMeshes[0].mVertices[2].normal = glm::vec3(0.0f, 1.0f, 0.0f);
		mMeshes[0].mVertices[3].normal = glm::vec3(0.0f, 1.0f, 0.0f);

		mMeshes[0].mVertices[0].tangent = glm::vec3(0.0f, 0.0f, 1.0f);
		mMeshes[0].mVertices[1].tangent = glm::vec3(0.0f, 0.0f, 1.0f);
		mMeshes[0].mVertices[2].tangent = glm::vec3(0.0f, 0.0f, 1.0f);
		mMeshes[0].mVertices[3].tangent = glm::vec3(0.0f, 0.0f, 1.0f);

		mMeshes[0].mVertices[0].bitangent = glm::vec3(1.0f, 0.0f, 0.0f);
		mMeshes[0].mVertices[1].bitangent = glm::vec3(1.0f, 0.0f, 0.0f);
		mMeshes[0].mVertices[2].bitangent = glm::vec3(1.0f, 0.0f, 0.0f);
		mMeshes[0].mVertices[3].bitangent = glm::vec3(1.0f, 0.0f, 0.0f);

		mMeshes[0].mVertices[0].texCoord = glm::vec2(0.0f, 1.0f);
		mMeshes[0].mVertices[1].texCoord = glm::vec2(0.0f, 0.0f);
		mMeshes[0].mVertices[2].texCoord = glm::vec2(1.0f, 1.0f);
		mMeshes[0].mVertices[3].texCoord = glm::vec2(1.0f, 0.0f);

		mMeshes[0].mIndices[0] = 0;
		mMeshes[0].mIndices[1] = 1;
		mMeshes[0].mIndices[2] = 2;
		mMeshes[0].mIndices[3] = 1;
		mMeshes[0].mIndices[4] = 3;
		mMeshes[0].mIndices[5] = 2;

		mMeshes[0].CreateBuffers(mDevice);
	}

	void Model::InitializeTexture(aiMaterial *mat, aiTextureType type, std::string typeName) {
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
			aiString str;
			mat->GetTexture(type, i, &str);
			mat->GetTextureCount(type);
			bool alreadyLoaded = false;

			for (auto path : mTexturePath) {
				if (std::strcmp(path.data, str.data) == 0) {
					alreadyLoaded = true;
					break;
				}
			}

			if (!alreadyLoaded) {
				mTexturePath.push_back(str);
				mTextureType.push_back(typeName);
			}
		}
	}

	void Model::LoadTexture() {
		for (unsigned int i = 0; i < mTexturePath.size(); i++) {
			std::string fileName = std::string(mTexturePath[i].data);
			fileName = mDirectory + '/' + fileName;
			std::cout << "texture path: " << fileName << std::endl;

			mTextures[i].CreateTexture(mDevice, fileName);
		}

	}

	void Model::ProcessNode(aiNode *node, const aiScene *scene, std::vector<Mesh> &meshes) {
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			auto index = node->mMeshes[i];
			aiMesh *mesh = scene->mMeshes[index];
			ProcessMesh(mesh, scene, meshes[index]);
		}
	}

	void Model::ProcessMesh(aiMesh *aimesh, const aiScene *scene, Mesh &mesh) {
		auto numVertices = aimesh->mNumVertices;

		mesh.mVertices.resize(numVertices);

		for (unsigned i = 0; i < numVertices; i++) {
			// vertex position
			mesh.mVertices[i].pos.x = aimesh->mVertices[i].x;
			mesh.mVertices[i].pos.y = aimesh->mVertices[i].y;
			mesh.mVertices[i].pos.z = aimesh->mVertices[i].z;

			// vertex normal
			mesh.mVertices[i].normal.x = aimesh->mNormals[i].x;
			mesh.mVertices[i].normal.y = aimesh->mNormals[i].y;
			mesh.mVertices[i].normal.z = aimesh->mNormals[i].z;

			// vertex tangent
			mesh.mVertices[i].tangent.x = aimesh->mTangents[i].x;
			mesh.mVertices[i].tangent.y = aimesh->mTangents[i].y;
			mesh.mVertices[i].tangent.z = aimesh->mTangents[i].z;

			// vertex bitangent
			mesh.mVertices[i].bitangent.x = aimesh->mBitangents[i].x;
			mesh.mVertices[i].bitangent.y = aimesh->mBitangents[i].y;
			mesh.mVertices[i].bitangent.z = aimesh->mBitangents[i].z;

			// vertex texture coordinate
			if (aimesh->mTextureCoords[0]) {
				mesh.mVertices[i].texCoord.x = aimesh->mTextureCoords[0][i].x;
				mesh.mVertices[i].texCoord.y = aimesh->mTextureCoords[0][i].y;
			}
			else mesh.mVertices[i].texCoord = glm::vec2(0.0f, 0.0f);
		}

		mesh.mIndices.clear();

		for (unsigned int i = 0; i < aimesh->mNumFaces; i++) {
			if (aimesh->mFaces[i].mNumIndices == 3) {
				mesh.mIndices.push_back(aimesh->mFaces[i].mIndices[0]);
				mesh.mIndices.push_back(aimesh->mFaces[i].mIndices[1]);
				mesh.mIndices.push_back(aimesh->mFaces[i].mIndices[2]);
			}
		}

		if (aimesh->mMaterialIndex >= 0) {
			aiMaterial *material = scene->mMaterials[aimesh->mMaterialIndex];

		}

		mesh.CreateBuffers(mDevice);

		if (aimesh->mMaterialIndex >= 0) {
			aiMaterial *mat = scene->mMaterials[aimesh->mMaterialIndex];

			BindTexture(mat, mesh, aiTextureType_DIFFUSE, "diffuseTexture");
			BindTexture(mat, mesh, aiTextureType_SPECULAR, "spectularTexture");
			BindTexture(mat, mesh, aiTextureType_HEIGHT, "normalTexture");
			//aiTextureType texType = aiTextureType_DIFFUSE;

			//for (unsigned int i = 0; i < mat->GetTextureCount(texType); i++) {
			//	aiString str;
			//	mat->GetTexture(texType, i, &str);
			//	for (unsigned int j = 0; j < mTexturePath.size(); j++) {
			//		if (std::strcmp(mTexturePath[j].C_Str(), str.C_Str()) == 0) {
			//			mesh.mTextureIdx.push_back(j);
			//			mesh.mTextureType.push_back(std::string("texture_diffuse"));
					//	std::cout << "texture index is: " << j << std::endl;
					//	std::cout << "texture path is: " << str.C_Str() << std::endl;
				//		break;
				//	}
				//}
			//}

		}
	}

	void Model::BindTexture(aiMaterial *mat, Mesh& mesh, aiTextureType textureType, std::string typeName) {
		for (unsigned int i = 0; i < mat->GetTextureCount(textureType); i++) {
			aiString str;
			mat->GetTexture(textureType, i, &str);
			for (unsigned int j = 0; j < mTexturePath.size(); j++) {
				if (std::strcmp(mTexturePath[j].C_Str(), str.C_Str()) == 0) {
					mesh.mTextureIdx.push_back(j);
					mesh.mTextureType.push_back(typeName);
					break;
				}
			}
		}

		if (mat->GetTextureCount(textureType) == 0) {
			mesh.mTextureIdx.push_back(-1);
			mesh.mTextureType.push_back(typeName);
		}
	}

	void Model::ProcessTexture(aiMaterial *mat, aiTextureType type, std::string typeName) {
		std::vector<unsigned int> textureIndices;

		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
			aiString str;
			mat->GetTexture(type, i, &str);
			mat->GetTextureCount(type);
			bool alreadyLoaded = false;

			for (unsigned int j = 0; j < mTexturePath.size(); j++) {
				if (std::strcmp(mTexturePath[j].data, str.data) == 0) {
					textureIndices.push_back(j);
					alreadyLoaded = true;
					break;
				}
			}

			if (!alreadyLoaded) {
				// generate texture image

				mTexturePath.push_back(str);
				mTextureType.push_back(typeName);
			}
		}
	}

	void Model::CleanUp() {
		for (auto tex : mTextures) {
			tex.CleanUp(mDevice);
		}

		for (auto mesh : mMeshes) {
			mesh.CleanUp(mDevice);
		}
	}

	std::vector<VkDescriptorImageInfo> Model::TextureInfo(size_t meshIdx) {
		std::vector<VkDescriptorImageInfo> imageInfo;
		imageInfo.resize(mMeshes[meshIdx].mTextureIdx.size());

		for (int i = 0; i < mMeshes[meshIdx].mTextureIdx.size(); i++) {
			int textureIdx = mMeshes[meshIdx].mTextureIdx[i];
			if (textureIdx < 0) textureIdx = 0;

			imageInfo[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo[i].imageView = mTextures[textureIdx].mTexImage.mImageView;
			imageInfo[i].sampler = mTextures[textureIdx].mSampler;
		}
		
		return imageInfo;
	}
}
