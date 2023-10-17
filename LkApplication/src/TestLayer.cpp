#include "TestLayer.h"
#include <LkEngine/UI/UILayer.h>
#include <LkEngine/Math/Math.h>
#include <LkEngine/Application.h>
#include <LkEngine/Renderer/Camera.h>
#include <LkEngine/Scene/EntityFactory.h>


// TODO: Move to a better location to be used for geometry
static float box_vertices[8] = {
  -0.5f, -0.5f,  // 0
   0.5f, -0.5f,  // 1
   0.5f,  0.5f,  // 2
  -0.5f,  0.5f   // 3
};

static unsigned int box_indices[6] = {
    0, 1, 2,
    2, 3, 0
};

TestLayer::TestLayer()
{
    m_Name = "TestLayer";
    ColorSlider = { 0.0f, 0.50f, 0.35f, 1.0f };
    RectPos = { 0.0f, 0.0f, 0.0f };

    auto app = LkEngine::Application::Get();
    float width = (float)app->GetWidth();
    float height = (float)app->GetHeight();

    Pos = glm::vec3(0.0f);
    Translation = glm::vec3(0.0f);
    Scale = glm::vec3(1.0f);
    Model = glm::mat4(1.0f);
    View = glm::mat4(1.0f);
    Projection = glm::ortho<float>(0, width, height, 0.0f, -1.0f, 1.0f);

    // 3D
    Yaw = 3.0f * glm::pi<float>() / 4.0f;
	Pitch = glm::pi<float>() / 4.0f;
	//Rot = glm::quat(glm::vec3(Pitch, Yaw, 0.0f));
    Rot = glm::quat();

    // For testing rectangles
    ScalerSlider1 = 1.0f;
    ScalerSlider2 = 1.0f;
}

TestLayer::~TestLayer()
{
}

void TestLayer::OnAttach()
{
    LOG_DEBUG("TestLayer::OnAttach()");
    m_VAO = new VertexArray();
    m_VBO = new VertexBuffer(box_vertices, LK_ARRAYSIZE(box_vertices));
    m_IBO = new IndexBuffer(box_indices, LK_ARRAYSIZE(box_indices));

    VertexBufferLayout layout;
    layout.Push<float>(2);
    m_VAO->AddBuffer(*m_VBO, layout);

    //m_Shader = new Shader("assets/shaders/basic_color.shader");
    m_Shader = new Shader("assets/shaders/basic_transform.shader");
    m_Shader->Bind();
    m_Shader->SetUniform4f("u_Color", ColorSlider.x, ColorSlider.y, ColorSlider.z, ColorSlider.w);
    glm::mat4 identity_mat = glm::mat4(1.0f);
    m_Shader->SetUniformMat4f("u_TransformMatrix", identity_mat);
    m_Shader->Unbind();

    m_Scene = create_s_ptr<Scene>();
    EntityFactory::CreateRectangle(*m_Scene, {0, 0}, {1, 1});
    EntityFactory::CreateRectangle(*m_Scene, {0, 0}, {1, 1});
    EntityFactory::CreateRectangle(*m_Scene, {0, 0}, {1, 1});
    EntityFactory::CreateRectangle(*m_Scene, {0, 0}, {1, 1});
}

void TestLayer::OnDetach()
{
}

void TestLayer::OnUpdate(float ts)
{
    m_Scene->OnUpdate(ts);

}

void TestLayer::OnImGuiRender()
{
    m_Scene->OnImGuiRender();
}



