#include "GameLayer.h"
#include "Enemy.h"

#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/SceneCamera.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/TextureLibrary.h"
#include "LkEngine/Renderer/Mesh.h"

#include "LkEngine/Debug/DebugLayer.h"

#include "LkEngine/Platform/OpenGL/OpenGLRenderer.h"

//#define DONT_SAVE_SCENE
#define USE_SAVED_SCENE

namespace LkEngine {

    static Ref<Shader> ModelShader;
    
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

        CubeMeshShader = Renderer::GetShaderLibrary()->Get("Renderer_Debug");
        CubeMeshShader->Set("u_Texture0", 0);
        DebugShader = Renderer::GetShaderLibrary()->Get("Renderer2D_Debug");
        DebugShader->Set("Texture1", 0);

        LK_CORE_WARN_TAG("GameLayer", "Creating Skybox");
		SkyboxVertexBuffer = VertexBuffer::Create(Skybox_Vertices, sizeof(Skybox_Vertices));
		SkyboxVertexBuffer->SetLayout({
            { "a_Position",      ShaderDataType::Float3  },
            //{ "a_Normal",        ShaderDataType::Float3  },
            //{ "a_Texcoord",      ShaderDataType::Float2  },
		});
        SkyboxShader = Renderer::GetShaderLibrary()->Get("Renderer_Skybox");

        // Skybox
        TextureSpecification skyboxSpec;
        std::vector<std::filesystem::path> facePaths = {
            "Assets/Textures/Skybox/right.jpg",
            "Assets/Textures/Skybox/left.jpg",
            "Assets/Textures/Skybox/top.jpg",
            "Assets/Textures/Skybox/bottom.jpg",
            "Assets/Textures/Skybox/front.jpg",
            "Assets/Textures/Skybox/back.jpg",
        };
        SkyboxTexture = TextureCube::Create(skyboxSpec, facePaths);
        SkyboxTexture->Bind(0);
        SkyboxShader->Bind();
        SkyboxShader->Set("u_Skybox", 0);
        LK_CORE_WARN_TAG("GameLayer", "Skybox created!");
    }

    void GameLayer::OnDetach()
    {
    }

    void GameLayer::OnUpdate(float ts)
    {
        auto entities = m_Scene->GetEntities();
        for (Entity& entity : entities)
        {
            if (entity.HasComponent<SpriteComponent>())
            {
                glm::vec2 size = { entity.Sprite().Size.x * entity.Transform().Scale.x, entity.Sprite().Size.y * entity.Transform().Scale.y };
                //Renderer::SubmitQuad(entity.Transform().Translation, size, entity.().GetTexture(), entity.GetUUID());
            }
            if (entity.HasComponent<MeshComponent>())
            {
                MeshComponent& mc = entity.Mesh();
                TransformComponent& tc = entity.Transform();
                Ref<Mesh> mesh = AssetManager::GetAsset<Mesh>(mc.Mesh);

                Renderer::Submit([mesh, tc]() mutable
                {
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, tc.Translation);

                    Ref<Material> material = mesh->GetMaterial(0);
                    Ref<Texture2D> materialTexture = material->GetTexture("wood-container");

                    Ref<Shader> matShader = material->GetShader();

                    materialTexture->Bind();
                    matShader->Bind();
                    matShader->Set("u_Model", model);
                    matShader->Set("u_ViewProjectionMatrix", EditorLayer::Get()->GetEditorCamera()->GetViewProjectionMatrix());

                    mesh->GetVertexBuffer()->Bind();
		            glDrawElements(GL_TRIANGLES, mesh->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
                });
            }
        }

		Renderer::GetViewportFramebuffer()->Bind();

        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        SkyboxShader->Bind();
        auto& camera = *EditorLayer::Get()->GetEditorCamera();
        glm::mat4 viewMatrix = glm::mat4(glm::mat3(camera.GetViewMatrix())); // Make TextureCube follow us
        glm::mat4 projectionMatrix = camera.GetProjectionMatrix();
        SkyboxShader->Set("u_ViewProjectionMatrix", projectionMatrix * viewMatrix);
        SkyboxShader->Set("u_CameraPos", EditorLayer::Get()->GetEditorCamera()->GetViewMatrix());
        SkyboxShader->Set("u_Model", glm::mat4(30));
        SkyboxVertexBuffer->Bind();
        SkyboxTexture->Bind(0);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS); 

        Framebuffer::TargetSwapChain();

#if 0
        // Crysis Nanosuit
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
#endif

    }

    void GameLayer::OnImGuiRender()
    {
    }


}