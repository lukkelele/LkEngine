#include "GameLayer.h"
#include "Enemy.h"

#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/SceneCamera.h"

#include "LkEngine/Renderer/Renderer.h"
//#include "LkEngine/Renderer/Renderer2D.h"
#include "LkEngine/Renderer/TextureLibrary.h"

#include "LkEngine/Debug/DebugLayer.h"

#include "LkEngine/Scene/Model.h"

#include "LkEngine/Platform/OpenGL/OpenGLRenderer.h"

//#define DONT_SAVE_SCENE
#define USE_SAVED_SCENE


namespace LkEngine {

    static Ref<Shader> ModelShader;

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

        Entity e1 = m_Scene->CreateEntity("lukas");
        // e1
        {
            auto& tc = e1.Transform();
            tc.Translation.x = 10;
            tc.Translation.y = 6;
            tc.Translation.z = 18;

            auto& sc = e1.AddComponent<SpriteComponent>();
            sc.SetSize(10, 10);
            sc.Color = Color::RGBA::Blue;

            auto& mc = e1.AddComponent<MaterialComponent>();
            mc.m_Material = Ref<Material>::Create();
            mc.m_Material->SetTexture(TextureLibrary::Get()->GetTexture2D("bricks"));
        }

        // e2
        Entity e2 = m_Scene->CreateEntity("ale wow");
        {
            auto& tc = e2.Transform();
            tc.Translation.x = -8;
            tc.Translation.y = 6;
            tc.Translation.z = 24;

            auto& sc = e2.AddComponent<SpriteComponent>();
            sc.SetSize(8, 8);
            sc.Color = Color::RGBA::Red;

            auto& mc = e2.AddComponent<MaterialComponent>();
            mc.m_Material = Ref<Material>::Create();
            mc.m_Material->SetTexture(TextureLibrary::Get()->GetTexture2D("container"));
        }

        //ModelShader = Shader::Create("assets/Shaders/OpenGL/Renderer_Model.shader");
        //m_BackpackModel = new Model("assets/Meshes/Template/Backpack/backpack.obj");
        //m_CubeModel = new Model("assets/Meshes/Cube.gltf");
        //m_CrisisNanosuit = new Model("assets/Meshes/Template/Nanosuit/nanosuit.obj");
    }

    void GameLayer::OnDetach()
    {
    }

    void GameLayer::OnUpdate(float ts)
    {
        auto entities = m_Scene->GetEntities();
        for (auto& entity : entities)
        {
            glm::vec2 size = { entity.Sprite().Size.x * entity.Transform().Scale.x, entity.Sprite().Size.y * entity.Transform().Scale.y };
            Renderer::SubmitQuad(entity.Transform().Translation, size, entity.Material().GetTexture(), entity.GetUUID());
        }

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