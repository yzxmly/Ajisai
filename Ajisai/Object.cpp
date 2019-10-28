#include "Object.h"
#include "stb_image.h"

namespace Ajisai {
	
	void Object::LoadObject(std::string filePath) {
		Assimp::Importer import;
		const aiScene *scene = import.ReadFile(filePath, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_PreTransformVertices);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "Assimp error: " << import.GetErrorString() << std::endl;
			return;
		}

		mDirectory = filePath.substr(0, filePath.find_last_of('\\'));
		std::cout << "Object Directory: " << mDirectory << std::endl;

		mMeshes.resize(scene->mNumMeshes);
		std::cout << "total mesh number: " << scene->mNumMeshes << std::endl;

		unsigned int textureSize = 0;
		for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
			auto mat = scene->mMaterials[i];
			
			InitializeTexture(mat, aiTextureType_DIFFUSE, "texture_diffuse");
			InitializeTexture(mat, aiTextureType_SPECULAR, "texture_specular");
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

	void Object::InitializeTexture(aiMaterial *mat, aiTextureType type, std::string typeName) {
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

	void Object::LoadTexture() {
		for (unsigned int i = 0; i < mTexturePath.size(); i++) {
			std::string fileName = std::string(mTexturePath[i].data);
			fileName = mDirectory + '/' + fileName;
			std::cout << "texture path: " << fileName << std::endl;

			mTextures[i].CreateTexture(mDevice, fileName);
		}
		
	}

	void Object::ProcessNode(aiNode *node, const aiScene *scene, std::vector<Mesh> &meshes) {
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			auto index = node->mMeshes[i];
			aiMesh *mesh = scene->mMeshes[index];
			ProcessMesh(mesh, scene, meshes[index]);
		}
	}

	void Object::ProcessMesh(aiMesh *aimesh, const aiScene *scene, Mesh &mesh) {
		auto numVertices = aimesh->mNumVertices;

		mesh.mVertices.resize(numVertices);

		for (unsigned i = 0; i < numVertices; i++) {
			// vertex position
			mesh.mVertices[i].pos.x = aimesh->mVertices[i].x;
			mesh.mVertices[i].pos.y = aimesh->mVertices[i].y;
			mesh.mVertices[i].pos.z = aimesh->mVertices[i].z;

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
	}

	void Object::ProcessTexture(aiMaterial *mat, aiTextureType type, std::string typeName) {
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

	void Object::CleanUp() {
		for (auto tex : mTextures) {
			tex.CleanUp(mDevice);
		}

		for (auto mesh : mMeshes) {
			mesh.CleanUp(mDevice);
		}
	}

	VkDescriptorImageInfo Object::TextureInfo(unsigned int meshIdx) {
		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		//imageInfo.imageView = 
		return imageInfo;
	}
}
