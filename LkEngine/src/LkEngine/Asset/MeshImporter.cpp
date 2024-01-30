#include "LKpch.h"
#include "MeshImporter.h"


namespace LkEngine {

	MeshImporter::MeshImporter()
	{
		m_Meshes.clear();
	}

	Mesh MeshImporter::Load(std::filesystem::path filepath)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath.string(), aiProcess_Triangulate | aiProcess_FlipUVs);

		if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
		{
			LK_CORE_ERROR_TAG("MeshImporter", "Assimp error: {}", importer.GetErrorString());
			LK_CORE_ASSERT(false, "Assimp error: {}", importer.GetErrorString());
		}

		ProcessNode(scene->mRootNode, scene);

		LK_CORE_DEBUG_TAG("MeshImporter", "Loaded meshes: {}", m_Meshes.size());
		return m_Meshes[0];
	}

	void MeshImporter::ProcessNode(aiNode* node, const aiScene* scene)
	{
		LK_CORE_VERIFY(node->mNumMeshes > 0, "aiNode has 0 meshes");
		// Process all the node's meshes (if any)
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
		    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]]; 
		    m_Meshes.push_back(ProcessMesh(mesh, scene));			
		}

		// Do the same for each of its children
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
		    ProcessNode(node->mChildren[i], scene);
		}
	}

	Mesh MeshImporter::ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<RendererID> textures;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
		    Vertex vertex;

			// Position
			glm::vec3 vec(0.0f);
			vec.x = mesh->mVertices[i].x;
			vec.y = mesh->mVertices[i].y;
			vec.z = mesh->mVertices[i].z;
			vertex.Position = vec;

			// Normals
			glm::vec3 normal(0.0f);
			if (mesh->HasNormals())
			{
				normal.x = mesh->mNormals[i].x;
				normal.y = mesh->mNormals[i].y;
				normal.z = mesh->mNormals[i].z;
			}
			vertex.Normal = normal; 

			// Texture coordinates
			if (mesh->mTextureCoords[0]) 
			{
			    glm::vec2 vec2;
			    vec2.x = mesh->mTextureCoords[0][i].x; 
			    vec2.y = mesh->mTextureCoords[0][i].y;
			    vertex.TexCoords = vec2;
			}
			else
			{
			    vertex.TexCoords = glm::vec2(0.0f, 0.0f);  
			}

		    vertices.push_back(vertex);
		}

		// Indices
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
		    for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		} 

		// Material
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			//vector<RendererID> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse"); textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			//vector<RendererID> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular"); textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		return Mesh(vertices, indices, textures);
	}

	std::vector<Ref<Texture>> MeshImporter::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
	{
		LK_CORE_INFO_TAG("MeshImporter", "Loading material textures");
		std::vector<Ref<Texture>> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
		    aiString str;
		    mat->GetTexture(type, i, &str);

			TextureSpecification spec;
			spec.Path = str.C_Str();
			spec.Name = mat->GetName().C_Str();
			spec.DebugName = mat->GetName().C_Str();

			LK_CORE_TRACE_TAG("MeshImporter", "aiMaterial->GetTexture() --> {}", str.C_Str());
		    textures.push_back(Texture::Create(spec));
		}
		return textures;
	}


}