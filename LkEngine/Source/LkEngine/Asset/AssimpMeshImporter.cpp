#include "LKpch.h"
#include "AssimpMeshImporter.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "AssimpLogStream.h"
#include "AssetManager.h"

#include "LkEngine/Renderer/Renderer.h"
#if defined(LK_ENGINE_OPENGL)
#include "LkEngine/Renderer/Backend/OpenGL/LkOpenGL.h"
#endif

namespace LkEngine {

	#define LK_MESH_LOG(...)    LK_CORE_TRACE_TAG("Mesh", __VA_ARGS__)
	#define LK_MESH_ERROR(...)  LK_CORE_ERROR_TAG("Mesh", __VA_ARGS__)

	static constexpr uint32_t MeshImportFlags =
		aiProcess_CalcTangentSpace | /* Create binormals/tangents just in case. */
		aiProcess_Triangulate |		 /* Make sure we're triangles. */
		aiProcess_SortByPType |		 /* Split meshes by primitive type. */
		aiProcess_GenNormals |		 /* Make sure we have legit normals. */
		aiProcess_GenUVCoords |		 /* Convert UVs if required. */
									 // aiProcess_OptimizeGraph |
		// aiProcess_OptimizeMeshes |          /* Batch draws where possible */
		// aiProcess_JoinIdenticalVertices |
		aiProcess_LimitBoneWeights |	  /* If more than N(= 4) bone weights, discard least influencing bones and re-normalise sum to 1. */
		aiProcess_ValidateDataStructure | /* Validation. */
		aiProcess_GlobalScale;			  /* E.g convert cm to m for FBX import and other formats where cm is native. */

	namespace Utils 
	{
		static glm::mat4 Mat4FromAIMatrix4x4(const aiMatrix4x4& Matrix)
		{
			/* Assimp uses A, B, C, D for the rows and 1, 2, 3, 4 for columns. */
			glm::mat4 Result{};
			Result[0][0] = Matrix.a1;
			Result[1][0] = Matrix.a2;
			Result[2][0] = Matrix.a3;
			Result[3][0] = Matrix.a4;

			Result[0][1] = Matrix.b1;
			Result[1][1] = Matrix.b2;
			Result[2][1] = Matrix.b3;
			Result[3][1] = Matrix.b4;

			Result[0][2] = Matrix.c1;
			Result[1][2] = Matrix.c2;
			Result[2][2] = Matrix.c3;
			Result[3][2] = Matrix.c4;

			Result[0][3] = Matrix.d1;
			Result[1][3] = Matrix.d2;
			Result[2][3] = Matrix.d3;
			Result[3][3] = Matrix.d4;

			return Result;
		}

		[[maybe_unused]] static void PrintNode(aiNode* node, std::size_t depth)
		{
			LK_MESH_LOG("{0:^{1}}{2} {{", "", depth * 3, node->mName.C_Str());
			depth++;
			glm::vec3 Translation;
			glm::quat RotationQuat;
			glm::vec3 Scale;
			glm::mat4 Transform = Mat4FromAIMatrix4x4(node->mTransformation);
			Math::DecomposeTransform(Transform, Translation, RotationQuat, Scale);

			const glm::vec3 Rotation = glm::degrees(glm::eulerAngles(RotationQuat));

			LK_MESH_LOG("{0:^{1}} Translation: ({2:6.2f}, {3:6.2f}, {4:6.2f})", "", depth * 3, Translation.x, Translation.y, Translation.z);
			LK_MESH_LOG("{0:^{1}} Rotation:    ({2:6.2f}, {3:6.2f}, {4:6.2f})", "", depth * 3, Rotation.x, Rotation.y, Rotation.z);
			LK_MESH_LOG("{0:^{1}} Scale:       ({2:6.2f}, {3:6.2f}, {4:6.2f})", "", depth * 3, Scale.x, Scale.y, Scale.z);
			for (uint32_t i = 0; i < node->mNumChildren; ++i)
			{
				PrintNode(node->mChildren[i], depth);
			}
			depth--;

			LK_MESH_LOG("{0:^{1}}}}", "", depth * 3);
		}

	}

	LAssimpMeshImporter::LAssimpMeshImporter(const std::filesystem::path& InFilePath)
		: FilePath(InFilePath)
	{
	}

	bool LAssimpMeshImporter::IsValid() const
	{
		/* TODO: Validate the filepath existance here. */
		return true;
	}

	TObjectPtr<LMeshSource> LAssimpMeshImporter::ImportToMeshSource()
	{
		LK_CORE_TRACE_TAG("AssimpMeshImporter", "Importing mesh source: {}", FilePath.string());
		TObjectPtr<LMeshSource> MeshSource = TObjectPtr<LMeshSource>::Create();

		Assimp::Importer Importer;
		Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

		/* An assimp scene can contain no meshes. Something like an armature, an animation and/or no skin. */
		const aiScene* AssimpScene = Importer.ReadFile(FilePath.string(), MeshImportFlags);
		if (!AssimpScene /* || !scene->HasMeshes()*/)
		{
			LK_CORE_ERROR_TAG("Mesh", "Failed to load mesh file: {}", FilePath.string());
			MeshSource->SetFlag(EAssetFlag::Invalid);
			return nullptr;
		}

		/* If no meshes in the scene, there's nothing more for us to do. */
		if (AssimpScene->HasMeshes())
		{
			uint32_t VertexCount = 0;
			uint32_t IndexCount = 0;

			MeshSource->BoundingBox.Min = { FLT_MAX, FLT_MAX, FLT_MAX };
			MeshSource->BoundingBox.Max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

			MeshSource->Submeshes.reserve(AssimpScene->mNumMeshes);
			LK_CORE_TRACE_TAG("AssimpMeshImporter", "Reserving spots for {} submeshes", AssimpScene->mNumMeshes);
			for (unsigned m = 0; m < AssimpScene->mNumMeshes; m++)
			{
				aiMesh* Mesh = AssimpScene->mMeshes[m];

				LSubmesh& Submesh = MeshSource->Submeshes.emplace_back();
				Submesh.BaseVertex = VertexCount;
				Submesh.BaseIndex = IndexCount;
				Submesh.MaterialIndex = Mesh->mMaterialIndex;
				Submesh.VertexCount = Mesh->mNumVertices;
				Submesh.IndexCount = Mesh->mNumFaces * 3;
				Submesh.MeshName = Mesh->mName.C_Str();

				VertexCount += Mesh->mNumVertices;
				IndexCount += Submesh.IndexCount;
				LK_CORE_TRACE_TAG("AssimpMeshImporter", "{}", Submesh.ToString());

				LK_CORE_VERIFY(Mesh->HasPositions(), "Meshes require positions");
				LK_CORE_VERIFY(Mesh->HasNormals(), "Meshes require normals");
				/* TODO: Add check for UV coordinates? */

				/* Vertices. */
				FAABB& BoundingBox = Submesh.BoundingBox;
				BoundingBox.Min = { FLT_MAX, FLT_MAX, FLT_MAX };
				BoundingBox.Max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

				for (size_t i = 0; i < Mesh->mNumVertices; i++)
				{
					FVertex Vertex{};
					Vertex.Position = {
						Mesh->mVertices[i].x,
						Mesh->mVertices[i].y,
						Mesh->mVertices[i].z
					};

					Vertex.Normal = {
						Mesh->mNormals[i].x,
						Mesh->mNormals[i].y,
						Mesh->mNormals[i].z
					};

				#if 0
					BoundingBox.Min.x = glm::min(Vertex.Position.x, BoundingBox.Min.x);
					BoundingBox.Min.y = glm::min(Vertex.Position.y, BoundingBox.Min.y);
					BoundingBox.Min.z = glm::min(Vertex.Position.z, BoundingBox.Min.z);
					BoundingBox.Max.x = glm::max(Vertex.Position.x, BoundingBox.Max.x);
					BoundingBox.Max.y = glm::max(Vertex.Position.y, BoundingBox.Max.y);
					BoundingBox.Max.z = glm::max(Vertex.Position.z, BoundingBox.Max.z);
				#else
					BoundingBox.Min.X = glm::min(Vertex.Position.x, BoundingBox.Min.X);
					BoundingBox.Min.Y = glm::min(Vertex.Position.y, BoundingBox.Min.Y);
					BoundingBox.Min.Z = glm::min(Vertex.Position.z, BoundingBox.Min.Z);
					BoundingBox.Max.X = glm::max(Vertex.Position.x, BoundingBox.Max.X);
					BoundingBox.Max.Y = glm::max(Vertex.Position.y, BoundingBox.Max.Y);
					BoundingBox.Max.Z = glm::max(Vertex.Position.z, BoundingBox.Max.Z);
				#endif

					if (Mesh->HasTangentsAndBitangents())
					{
						/* Tangent. */
						Vertex.Tangent = {
							Mesh->mTangents[i].x,
							Mesh->mTangents[i].y,
							Mesh->mTangents[i].z
						};

						/* Binormal. */
						Vertex.Binormal = {
							Mesh->mBitangents[i].x,
							Mesh->mBitangents[i].y,
							Mesh->mBitangents[i].z
						};
					}

					if (Mesh->HasTextureCoords(0))
					{
						// Vertex.Texcoord = { 2 * Mesh->mTextureCoords[0][i].x, 2 * Mesh->mTextureCoords[0][i].y };
						Vertex.Texcoord = {
							Mesh->mTextureCoords[0][i].x,
							Mesh->mTextureCoords[0][i].y
						};
					}

					MeshSource->Vertices.push_back(Vertex);
				}

				/* Indices. */
				for (size_t i = 0; i < Mesh->mNumFaces; i++)
				{
					LK_CORE_ASSERT(Mesh->mFaces[i].mNumIndices == 3, "Mesh must have 3 indices");
					FIndex Index = {
						Mesh->mFaces[i].mIndices[0],
						Mesh->mFaces[i].mIndices[1],
						Mesh->mFaces[i].mIndices[2]
					};
					MeshSource->Indices.push_back(Index);

					/* TODO: Fix overflow warning. */
					MeshSource->TriangleCache[m].emplace_back(
						MeshSource->Vertices[Index.V1 + Submesh.BaseVertex],
						MeshSource->Vertices[Index.V2 + Submesh.BaseVertex],
						MeshSource->Vertices[Index.V3 + Submesh.BaseVertex]
					);
				}
			}

			FMeshNode& RootNode = MeshSource->MeshNodes.emplace_back();
			TraverseNodes(MeshSource, AssimpScene->mRootNode, 0);

			for (const LSubmesh& Submesh : MeshSource->Submeshes)
			{
				const FAABB TransformedSubmeshAABB = Submesh.BoundingBox;
			#if 0
				const glm::vec3 Min = glm::vec3(Submesh.Transform * glm::vec4(TransformedSubmeshAABB.Min, 1.0f));
				const glm::vec3 Max = glm::vec3(Submesh.Transform * glm::vec4(TransformedSubmeshAABB.Max, 1.0f));
			#else
				const glm::vec3 Min = glm::vec3(Submesh.Transform * glm::vec4(TransformedSubmeshAABB.Min.As<glm::vec3>(), 1.0f));
				const glm::vec3 Max = glm::vec3(Submesh.Transform * glm::vec4(TransformedSubmeshAABB.Max.As<glm::vec3>(), 1.0f));
			#endif

			#if 0
				MeshSource->BoundingBox.Min.x = glm::min(MeshSource->BoundingBox.Min.x, Min.x);
				MeshSource->BoundingBox.Min.y = glm::min(MeshSource->BoundingBox.Min.y, Min.y);
				MeshSource->BoundingBox.Min.z = glm::min(MeshSource->BoundingBox.Min.z, Min.z);
				MeshSource->BoundingBox.Max.x = glm::max(MeshSource->BoundingBox.Max.x, Max.x);
				MeshSource->BoundingBox.Max.y = glm::max(MeshSource->BoundingBox.Max.y, Max.y);
				MeshSource->BoundingBox.Max.z = glm::max(MeshSource->BoundingBox.Max.z, Max.z);
			#else
				MeshSource->BoundingBox.Min.X = glm::min(MeshSource->BoundingBox.Min.X, Min.x);
				MeshSource->BoundingBox.Min.Y = glm::min(MeshSource->BoundingBox.Min.Y, Min.y);
				MeshSource->BoundingBox.Min.Z = glm::min(MeshSource->BoundingBox.Min.Z, Min.z);
				MeshSource->BoundingBox.Max.X = glm::max(MeshSource->BoundingBox.Max.X, Max.x);
				MeshSource->BoundingBox.Max.Y = glm::max(MeshSource->BoundingBox.Max.Y, Max.y);
				MeshSource->BoundingBox.Max.Z = glm::max(MeshSource->BoundingBox.Max.Z, Max.z);
			#endif
			}
		}
		/* No meshes. */
		else
		{
			LK_CORE_ERROR_TAG("AssimpMeshImporter", "No meshes found when trying to import");
		}

		/* TODO: Check if vectors are empty or just crash? */
		LK_CORE_ASSERT(!MeshSource->Vertices.empty(), "MeshSource vertices are empty");
		LK_CORE_ASSERT(!MeshSource->Indices.empty(), "MeshSource indices are empty");

		if (!MeshSource->Vertices.empty())
		{
			MeshSource->VertexBuffer = LVertexBuffer::Create(
				MeshSource->Vertices.data(), 
				(uint64_t)(MeshSource->Vertices.size() * sizeof(FVertex))
			);
		}

		if (!MeshSource->Indices.empty())
		{
			MeshSource->IndexBuffer = LIndexBuffer::Create(
				MeshSource->Indices.data(), 
				(uint64_t)(MeshSource->Indices.size() * sizeof(FIndex))
			);
		}

		return MeshSource;
	}

	void LAssimpMeshImporter::TraverseNodes(TObjectPtr<LMeshSource> MeshSource,
											void* InAssimpNode,
											uint32_t NodeIndex,
											const glm::mat4& ParentTransform,
											const uint32_t Level)
	{
		aiNode* AssimpNode = static_cast<aiNode*>(InAssimpNode);

		FMeshNode& MeshNode = MeshSource->MeshNodes[NodeIndex];
		MeshNode.Name = AssimpNode->mName.C_Str();
		MeshNode.LocalTransform = Utils::Mat4FromAIMatrix4x4(AssimpNode->mTransformation);

		const glm::mat4 Transform = ParentTransform * MeshNode.LocalTransform;
		for (uint32_t i = 0; i < AssimpNode->mNumMeshes; i++)
		{
			const uint32_t SubmeshIndex = AssimpNode->mMeshes[i];
			LSubmesh& Submesh = MeshSource->Submeshes[SubmeshIndex];
			Submesh.NodeName = AssimpNode->mName.C_Str();
			Submesh.Transform = Transform;
			Submesh.LocalTransform = MeshNode.LocalTransform;

			MeshNode.Submeshes.push_back(SubmeshIndex);
		}

		uint32_t ParentNodeIndex = static_cast<uint32_t>(MeshSource->MeshNodes.size() - 1);

		MeshNode.Children.resize(AssimpNode->mNumChildren);
		for (uint32_t i = 0; i < AssimpNode->mNumChildren; i++)
		{
			FMeshNode& ChildMeshNode = MeshSource->MeshNodes.emplace_back();
			const uint32_t ChildIndex = static_cast<uint32_t>(MeshSource->MeshNodes.size()) - 1;
			ChildMeshNode.Parent = ParentNodeIndex;
			MeshSource->MeshNodes[NodeIndex].Children[i] = ChildIndex;

			TraverseNodes(MeshSource,
						  AssimpNode->mChildren[i],
						  ChildIndex,
						  Transform,
						  Level + 1);
		}
	}

}
