#include "LKpch.h"
#include "MaterialEditor.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/Shader.h"

#include "LkEngine/Asset/AssetImporter.h"
#include "LkEngine/Asset/AssetManager.h"
#include "LkEngine/Asset/EditorAssetManager.h"

#include "LkEngine/Project/Project.h"

#include "LkEngine/Serialization/Serializer.h"


namespace LkEngine {

	namespace 
	{
		bool bWindow_ShaderSelection = false;
	}

	LMaterialEditor::LMaterialEditor()
		: LAssetEditor("Material Editor")
	{
		LOBJECT_REGISTER();

		/* TODO: Should be set to nullptr by default. */
		MaterialAsset = LEditorAssetManager::BaseMaterial;
	}

	void LMaterialEditor::Tick(const float DeltaTime)
	{
	}

	void LMaterialEditor::Render()
	{
		if (!MaterialAsset)
		{
			return;
		}

		TObjectPtr<LMaterial> Material = MaterialAsset->GetMaterial();

		UI::Font::Push("TitleBoldItalic");
		ImGui::Text("Material Editor");
		UI::Font::Pop();

		ImGui::Dummy(ImVec2(0, 8));

		UI::Font::Push("LargeBold");
		ImGui::Text("%s", Material->GetName().c_str());
		UI::Font::Pop();

		ImGui::Dummy(ImVec2(0, 10));

		UI::Font::Push("Bold");
		ImGui::Text("Shader: %s", Material->GetShader()->GetName().c_str());
		UI::Font::Pop();

		ImGui::Dummy(ImVec2(0, 6));

		TObjectPtr<LShader> Shader = Material->GetShader();
		bool bNeedsSerialize = false;
		static bool bRefreshUniformContainer = true;

		static std::vector<std::pair<EUniformVarType, std::string>> ShaderUniforms;
		auto& ShaderMap = LRenderer::GetShaderLibrary()->ShaderMap;
		const FShaderProgramSource ProgramSource = Shader->GetSource();

		std::string Line;
		std::stringstream StrStream(ProgramSource.Fragment);
		while (getline(StrStream, Line))
		{
			auto [VarType, UniformName] = LShader::GetUniformVariable(Line);
			if (VarType != EUniformVarType::None)
			{
				if (bRefreshUniformContainer)
				{
					LK_CORE_CONSOLE_DEBUG("Found uniform: {}", UniformName);
					ShaderUniforms.emplace_back(VarType, UniformName);
				}
			}
		}

		UI::BeginPropertyGrid();
		UI::PushID();

		for (const auto& [UniformType, UniformName] : ShaderUniforms)
		{
			if (UniformType == EUniformVarType::Bool)
			{
				bool Value = 0;
				Shader->Get(UniformName, Value);
				if (UI::Property(UniformName.c_str(), Value))
				{
					LK_CORE_INFO("Set uniform bool: {}", Value);
					Shader->Set(UniformName, Value);
				}
			}
			else if (UniformType == EUniformVarType::Float)
			{
				float Value = 0;
				Shader->Get(UniformName, Value);

				float FloatValue = Value;
				if (UI::Property(UniformName.c_str(), FloatValue, 0.010f, -50.0f, 50.0f))
				{
					Shader->Set(UniformName, static_cast<decltype(Value)>(FloatValue));
				}
			}
			else if (UniformType == EUniformVarType::Vec3)
			{
				glm::vec3 Value{};
				Shader->Get(UniformName, Value);

				glm::vec3 VecValue = Value;
				if (UI::Property(UniformName.c_str(), VecValue, 0.010f, -50.0f, 50.0f))
				{
					Shader->Set(UniformName, static_cast<decltype(Value)>(VecValue));
				}
			}
			else if (UniformType == EUniformVarType::Sampler2D)
			{
				uint32_t Value = 0;
				Shader->Get(UniformName, Value);

				float IntValue = Value;
				if (UI::Property(UniformName.c_str(), IntValue, 1.0f, 0.0f, 15.0f))
				{
					Shader->Set(UniformName, static_cast<decltype(Value)>(IntValue));
				}
			}

		}

		UI::PopID();
		UI::EndPropertyGrid();

	#if 0
		/* Uniform manipulation. */
		{
			static constexpr ImGuiTableFlags TableFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
			if (ImGui::BeginTable("##MaterialEditor-Uniforms", 2, TableFlags))
			{
				ImGui::TableSetupColumn("Uniform", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 180.0f);
				ImGui::TableHeadersRow();

				for (const auto& [UniformType, UniformName] : ShaderUniforms)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s", UniformName.c_str());

					ImGui::TableSetColumnIndex(1);
					if (UniformType == EUniformVarType::Bool)
					{
						bool Value = 0;
						Shader->Get(UniformName, Value);
						ImGui::Text("%d", Value);

						ImGui::SameLine(0, 5.0f);
						//Value ? UI::FScopedColor(ImGuiCol_Button, RGBA32::BrightGreen) : UI::FScopedColor(ImGuiCol_Button, RGBA32::BackgroundDark);
						ImGui::PushStyleColor(ImGuiCol_Button, (Value ? RGBA32::BrightGreen : RGBA32::BackgroundDark));
						if (ImGui::Button("Toggle"))
						{
							Shader->Set(UniformName, !Value);
						}
						ImGui::PopStyleColor();
					}
					else if (UniformType == EUniformVarType::Float)
					{
						float Value = 0;
						Shader->Get(UniformName, Value);

						float FloatValue = Value;
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						ImGui::SliderFloat(std::format("##{}", UniformName).c_str(), &FloatValue, -1.0f, 1.0f, "%.2f", ImGuiSliderFlags_None);
						if (FloatValue != Value)
						{
							Shader->Set(UniformName, static_cast<decltype(Value)>(FloatValue));
						}
					}
					else if (UniformType == EUniformVarType::Vec3)
					{
						glm::vec3 Value{};
						Shader->Get(UniformName, Value);

						glm::vec3 VecValue = Value;
						ImGui::SetNextItemWidth(180.0f);
						ImGui::SliderFloat3(std::format("##{}", UniformName).c_str(), &VecValue.x, -10.0f, 10.0f, "%.2f", ImGuiSliderFlags_None);
						if (VecValue != Value)
						{
							Shader->Set(UniformName, static_cast<decltype(Value)>(VecValue));
						}
					}
					else if (UniformType == EUniformVarType::Sampler2D)
					{
						uint32_t Value = 0;
						Shader->Get(UniformName, Value);

						int IntValue = Value;
						ImGui::SetNextItemWidth(130.0f);
						ImGui::SliderInt(std::format("##{}", UniformName).c_str(), &IntValue, 0, 5, "%d", ImGuiSliderFlags_None);
						if (IntValue != Value)
						{
							Shader->Set(UniformName, static_cast<decltype(Value)>(IntValue));
						}
					}
					else if (UniformType == EUniformVarType::Sampler2DArray)
					{
						uint32_t Value = 0;
						Shader->Get(UniformName, Value);
						ImGui::Text("%d (TODO)", Value);
					}
				}
				ImGui::EndTable();
			}
		}
	#endif

		if (bNeedsSerialize)
		{
			LAssetImporter::Serialize(MaterialAsset);
		}

		bRefreshUniformContainer = false;
	}

	void LMaterialEditor::OnOpen()
	{
		if (!MaterialAsset)
		{
			LK_CORE_WARN_TAG("MaterialEditor", "No material asset, closing editor");
			SetOpen(false);
		}
	}

	void LMaterialEditor::OnClose()
	{
		LK_CORE_DEBUG_TAG("MaterialEditor", "OnClose");
		MaterialAsset = LEditorAssetManager::BaseMaterial;
		//MaterialAsset = nullptr;
	}

	void LMaterialEditor::UI_ShaderSelection()
	{
		TObjectPtr<LShaderLibrary> ShaderLibrary = LRenderer::GetShaderLibrary();
		static std::array<const char*, 50> ShaderNames;
		LK_CORE_ASSERT(ShaderLibrary->ShaderMap.size() <= ShaderNames.size(), "Array overflow");

		static int32_t SelectedIdx = 0;
		static bool bRefreshUniformContainer = true;
		static std::vector<std::pair<EUniformVarType, std::string>> ShaderUniforms;

		auto& ShaderMap = ShaderLibrary->ShaderMap;

		int Idx = 0;
		for (auto& [ShaderName, Shader] : ShaderMap)
		{
			ShaderNames[Idx] = ShaderName.c_str();
			Idx++;
		}

		if (UI::PropertyDropdown("Shaders", ShaderNames.data(), ShaderMap.size(), &SelectedIdx))
		{
			LK_CORE_INFO("PropertyDropdown->Shaders: {}", (int)(SelectedIdx));
			bRefreshUniformContainer = true;
		}

		if (bRefreshUniformContainer)
		{
			ShaderUniforms.clear();
			LK_CORE_DEBUG_TAG("MaterialEditorPanel", "Cleared uniforms container");
		}

		if ((SelectedIdx >= 0) && (SelectedIdx < ShaderMap.size()))
		{
			TObjectPtr<LShader>& Shader = ShaderMap.at(ShaderNames[SelectedIdx]);
			const FShaderProgramSource ProgramSource = Shader->GetSource();

			std::string Line;
			std::stringstream StrStream(ProgramSource.Fragment);
			while (getline(StrStream, Line))
			{
				auto [VarType, UniformName] = LShader::GetUniformVariable(Line);
				if (VarType != EUniformVarType::None)
				{
					if (bRefreshUniformContainer)
					{
						LK_CORE_DEBUG_TAG("MaterialEditorPanel", "Adding to uniforms container: {} (Line: {})", UniformName, Line);
						ShaderUniforms.emplace_back(VarType, UniformName);
					}
				}
			}

			if (ImGui::TreeNode("Shader Program Source"))
			{
				ImGui::SetNextItemOpen(true, ImGuiCond_Once);
				if (ImGui::TreeNode("Vertex"))
				{
					ImGui::Text("%s", ProgramSource.Vertex.c_str());
					ImGui::TreePop();
				}
				
				ImGui::SetNextItemOpen(true, ImGuiCond_Once);
				if (ImGui::TreeNode("Fragment"))
				{
					ImGui::Text("%s", ProgramSource.Fragment.c_str());
					ImGui::TreePop();
				}

				ImGui::TreePop();
			}

			/* Uniform manipulation. */
			{
				static constexpr ImGuiTableFlags TableFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
				if (ImGui::BeginTable("##MaterialEditor-Uniforms", 2, TableFlags))
				{
					ImGui::TableSetupColumn("Uniform", ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 150.0f);
					ImGui::TableHeadersRow();

					for (const auto& [UniformType, UniformName] : ShaderUniforms)
					{
						ImGui::TableNextRow();

						ImGui::TableSetColumnIndex(0);
						ImGui::Text("%s", UniformName.c_str());

						ImGui::TableSetColumnIndex(1);
						if (UniformType == EUniformVarType::Bool)
						{
							bool Value = 0;
							Shader->Get(UniformName, Value);
							ImGui::Text("%d", Value);

							ImGui::SameLine(0, 5.0f);
							//Value ? UI::FScopedColor(ImGuiCol_Button, RGBA32::BrightGreen) : UI::FScopedColor(ImGuiCol_Button, RGBA32::BackgroundDark);
							ImGui::PushStyleColor(ImGuiCol_Button, (Value ? RGBA32::BrightGreen : RGBA32::BackgroundDark));
							if (ImGui::Button("Toggle"))
							{
								Shader->Set(UniformName, !Value);
							}
							ImGui::PopStyleColor();
						}
						else if (UniformType == EUniformVarType::Float)
						{
							float Value = 0;
							Shader->Get(UniformName, Value);

							float FloatValue = Value;
							static constexpr ImGuiInputTextFlags InputFlags = ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CharsDecimal;
							ImGui::SetNextItemWidth(130.0f);
							//ImGui::InputFloat(std::format("##{}", UniformName).c_str(), &FloatValue, 0.010f, 0.0f, "%.3f", InputFlags);
							ImGui::SliderFloat(std::format("##{}", UniformName).c_str(), &FloatValue, -10.0f, 10.0f, "%.2f", ImGuiSliderFlags_WrapAround);
							if (FloatValue != Value)
							{
								Shader->Set(UniformName, static_cast<decltype(Value)>(FloatValue));
							}
						}
						else if (UniformType == EUniformVarType::Sampler2D)
						{
							uint32_t Value = 0;
							Shader->Get(UniformName, Value);

							int IntValue = Value;
							static constexpr ImGuiInputTextFlags InputFlags = ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CharsDecimal;
							ImGui::SetNextItemWidth(130.0f);
							//ImGui::InputInt(std::format("##{}", UniformName).c_str(), &IntValue, 1, 5, InputFlags);
							ImGui::SliderInt(std::format("##{}", UniformName).c_str(), &IntValue, -10, 10, "%d", ImGuiSliderFlags_WrapAround);
						#if 0
							if (IntValue < 0)
							{
								LK_CORE_WARN_TAG("MaterialEditorPanel", "IntValue is not allowed to be less than 0");
								IntValue = 0;
							}
						#endif
							if (IntValue != Value)
							{
								Shader->Set(UniformName, static_cast<decltype(Value)>(IntValue));
							}
						}
						else if (UniformType == EUniformVarType::Sampler2DArray)
						{
							uint32_t Value = 0;
							Shader->Get(UniformName, Value);
							ImGui::Text("%d (TODO)", Value);
						}
						else
						{
							ImGui::Text("Unknown uniform type");
						}
					}
					ImGui::EndTable();
				}
			}

			ImGui::Separator();
		}

		bRefreshUniformContainer = false;
	}

	void LMaterialEditor::SerializeToYaml(YAML::Emitter& Out) const
	{
		LK_CORE_TRACE_TAG("MaterialEditor", "Serializing to YAML");
		Out << YAML::Key << "Data" << YAML::Value << YAML::BeginMap;
		{
			if (MaterialAsset && MaterialAsset->IsValid() && !LAssetManager::IsMemoryAsset(MaterialAsset->Handle))
			{
				LK_SERIALIZE_PROPERTY(MaterialAsset, MaterialAsset->Handle, Out);
			}
			else
			{
				LK_SERIALIZE_PROPERTY(MaterialAsset, (FAssetHandle)0, Out);
			}

			LK_SERIALIZE_PROPERTY(ShaderSelection, bWindow_ShaderSelection, Out);
		}
		Out << YAML::EndMap;
	}

	void LMaterialEditor::DeserializeFromYaml(const YAML::Node& EditorNode)
	{
		const YAML::Node& DataNode = EditorNode["Data"];
		if (!DataNode)
		{
			LK_CORE_ERROR_TAG("MaterialEditor", "Node 'Data' is not valid");
			return;
		}

		FAssetHandle MaterialHandle = 0;
		LK_DESERIALIZE_PROPERTY(MaterialAsset, MaterialHandle, DataNode, (FAssetHandle)0);
		if (MaterialHandle > 0)
		{
			TObjectPtr<LAsset> Asset = LProject::GetEditorAssetManager()->GetAsset(MaterialHandle);
			if (Asset)
			{
				LK_CORE_DEBUG_TAG("MaterialEditor", "Setting material asset to: {}", Asset->Handle);
				MaterialAsset = Asset.As<LMaterialAsset>();
			}
			else
			{
				LK_CORE_ERROR_TAG("MaterialEditor", "Failed to deserialize material asset: {}", MaterialHandle);
			}
		}

		LK_DESERIALIZE_PROPERTY(ShaderSelection, bWindow_ShaderSelection, DataNode, false);
	}

}
