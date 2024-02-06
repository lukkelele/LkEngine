#include "GameLayer.h"
#include "Enemy.h"

#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/SceneCamera.h"

#include "LkEngine/Renderer/Renderer.h"
//#include "LkEngine/Renderer/Renderer2D.h"
#include "LkEngine/Renderer/TextureLibrary.h"
#include "LkEngine/Renderer/Mesh.h"

#include "LkEngine/Debug/DebugLayer.h"

#include "LkEngine/Scene/Model.h"

#include "LkEngine/Platform/OpenGL/OpenGLRenderer.h"

//#define DONT_SAVE_SCENE
#define USE_SAVED_SCENE

namespace LkEngine {

    static Ref<Shader> ModelShader;
    
    Model* CubeObj;

    Ref<Mesh> CubeMesh;
    Ref<MeshSource> CubeMeshSource;
    Ref<Shader> CubeMeshShader;
    glm::vec3 CubeMeshTranslation{};

    GameLayer::GameLayer()
        : Layer("GameLayer")
    {
    }

    GameLayer::~GameLayer()
    {
    }

    void GameLayer::OnAttach()
    {
        m_Scene = EditorLayer::Get()->GetCurrentScene();
        LK_CORE_INFO_TAG("GameLayer", "Retrieved scene from Editor --> {}", m_Scene->GetName());

        Entity e1 = m_Scene->CreateEntity("Entity1");
        // e1
        {
            auto& tc = e1.Transform();
            tc.Translation.x = 10;
            tc.Translation.y = 6;
            tc.Translation.z = 18;

            auto& sc = e1.AddComponent<SpriteComponent>();
            sc.SetSize(10, 10);
            sc.Color = Color::RGBA::Blue;

            //auto& mc = e1.AddComponent<MaterialComponent>();
            //mc.m_Material = Ref<Material>::Create();
            //mc.m_Material->SetTexture(TextureLibrary::Get()->GetTexture2D("bricks"));
        }

        // e2
        Entity e2 = m_Scene->CreateEntity("Entity2");
        {
            auto& tc = e2.Transform();
            tc.Translation.x = -8;
            tc.Translation.y = 6;
            tc.Translation.z = 24;

            auto& sc = e2.AddComponent<SpriteComponent>();
            sc.SetSize(8, 8);
            sc.Color = Color::RGBA::Red;
        }

        DebugShader = Renderer::GetShaderLibrary()->Get("Renderer2D_Debug");
        CubeTexture = TextureLibrary::Get()->GetTexture2D("brickwall");
        DebugShader->Set("Texture1", 0);
        Ref<Material> bricksMaterial = Material::Create(CubeMeshShader, "Brickwall");

        Ref<AssetManager> assetManager = Project::GetAssetManager();
        AssetHandle cubeHandle = assetManager->GetAssetHandleFromFilePath("assets/Meshes/Cube.obj");
        CubeMesh = assetManager->GetAsset(cubeHandle).As<Mesh>();

        CubeMeshSource = CubeMesh->GetMeshSource();
        CubeMeshSource->m_VertexBuffer->SetLayout({
            { "a_Position",      ShaderDataType::Float3  },
            { "a_Normal",        ShaderDataType::Float3  },
            { "a_Tangent",       ShaderDataType::Float3  },
            { "a_Binormal",      ShaderDataType::Float3  },
            { "a_Texcoord",      ShaderDataType::Float2  },
            });
        CubeMeshSource->m_VertexBuffer->SetIndexBuffer(CubeMeshSource->m_IndexBuffer);
        CubeMeshShader = Renderer::GetShaderLibrary()->Get("Renderer_Debug");
        CubeMeshShader->Set("u_Texture0", 0);

        CubeMeshTranslation = { 0.0f, 0.0f, 0.0f };
    }

    void GameLayer::OnDetach()
    {
    }

    void GameLayer::OnUpdate(float ts)
    {
        auto entities = m_Scene->GetEntities();
        for (auto& entity : entities)
        {
            if (entity.HasComponent<SpriteComponent>())
            {
                glm::vec2 size = { entity.Sprite().Size.x * entity.Transform().Scale.x, entity.Sprite().Size.y * entity.Transform().Scale.y };
                //Renderer::SubmitQuad(entity.Transform().Translation, size, entity.().GetTexture(), entity.GetUUID());
            }
        }

        auto camera = EditorLayer::Get()->GetEditorCamera();
        glm::mat4 viewMatrix = EditorLayer::Get()->GetEditorCamera()->GetViewMatrix();
        glm::mat4 projMatrix = EditorLayer::Get()->GetEditorCamera()->GetProjectionMatrix();
        glm::mat4 viewProj = EditorLayer::Get()->GetEditorCamera()->GetViewProjectionMatrix();

        Renderer::Submit([camera]() 
        {
            glm::mat4 view = camera->GetViewMatrix();
            glm::mat4 projection = camera->GetProjectionMatrix();

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, CubeMeshTranslation);

            CubeMeshShader->Bind();
            CubeMeshShader->Set("u_Model", model);
            CubeMeshShader->Set("u_ViewProjectionMatrix", camera->GetViewProjectionMatrix());

            CubeMeshSource->GetVertexBuffer()->Bind();
		    CubeTexture->Bind(0);

            auto& ib = *CubeMeshSource->GetVertexBuffer()->GetIndexBuffer();
		    glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
            DebugShader->Unbind();
        });

        CubeMeshTranslation.x += 0.020f;
        CubeMeshTranslation.y -= 0.010f;
        

#if 0
        Renderer::Submit([camera]() 
        {
            glm::mat4 view = camera->GetViewMatrix();
            glm::mat4 projection = camera->GetProjectionMatrix();

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));

            DebugShader->Bind();
            DebugShader->Set("u_Model", model);
            DebugShader->Set("u_ViewMatrix", view);
            DebugShader->Set("u_ProjectionMatrix", projection);
            //DebugShader->Set("u_ViewProjectionMatrix", viewProjection);

		    //CubeVertexBuffer->Bind();
            CubeObj->GetVertexBuffer(0)->Bind();
		    CubeTexture->Bind(0);

            glDrawArrays(GL_TRIANGLES, 0, 36);

            DebugShader->Unbind();
        });
#endif

#if 0
        Renderer::Submit([&]()
        {
            auto& camera = *m_Scene->GetEditorCamera();
            glm::mat4 view = camera.GetViewMatrix();
            ModelShader->Bind();
            ModelShader->Set("u_ProjectionMatrix", camera.GetProjectionMatrix());
            ModelShader->Set("u_ViewMatrix", view);
            ModelShader->Unbind();
        });


        Renderer::Submit([&]()
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, -20.0f, 60.0f)); 
            model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));	
            ModelShader->Bind();
            ModelShader->Set("u_Model", model);
            m_CrisisNanosuit->Draw(ModelShader);
            ModelShader->Unbind();
        });

        Renderer::Submit([&]()
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, -20.0f, 60.0f)); 
            model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));	
            ModelShader->Bind();
            ModelShader->Set("u_Model", model);
            CubeVertexBuffer->Bind();
            glDrawArrays(GL_TRIANGLES, 0, 36);
            ModelShader->Unbind();
        });


        Renderer::Submit([&]()
        {
            CubeVertexBuffer->Bind();
            glDrawArrays(GL_TRIANGLES, 0, 36);
        });
#endif
    }

    void GameLayer::OnImGuiRender()
    {
    }


}