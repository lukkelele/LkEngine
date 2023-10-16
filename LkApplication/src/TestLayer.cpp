#include "TestLayer.h"
#include <LkEngine/UI/UI.h>
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

    auto camera = m_Scene->GetEditorCamera();
    float rot = camera->GetRotation();
    camera->UpdateView();
    auto& pos = camera->GetPos();

    m_Shader->Bind();
    m_Shader->SetUniform4f("u_Color", ColorSlider.x, ColorSlider.y, ColorSlider.z, ColorSlider.w);

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) 
                          * glm::rotate(glm::mat4(1.0f), glm::radians(rot), glm::vec3(0, 0, 1));
    transform = glm::scale(transform, { ScalerSlider1, ScalerSlider1, ScalerSlider1 });
    m_Shader->SetUniformMat4f("u_TransformMatrix", transform);
    Renderer::Draw(*m_VAO, *m_IBO, *m_Shader);

    //Entity rect1 = m_Scene->FindEntity("rect_1");
    //TransformComponent& rect_transform = rect1.GetComponent<TransformComponent>();
    //rect_transform.Scale = { ScalerSlider2, ScalerSlider2, ScalerSlider2 };
    // //LOG_DEBUG("Rect Transform Scale: {}", rect_transform.Scale.x);
    //MeshComponent& mesh = rect1.GetComponent<MeshComponent>();
    //mesh.Shader->Bind();
    //mesh.Shader->SetUniformMat4f("u_TransformMatrix", rect_transform.GetTransform());
    // LK_ASSERT_MESHCOMPONENT(mesh);
    //Renderer::Draw(*mesh.VAO, *mesh.IBO, *mesh.Shader);
}

void TestLayer::OnImGuiRender()
{
    ImGui::Begin(LkEngine::SIDEBAR_LEFT);

    DrawColorSliders();
    DrawPositionSliders();

    ImGui::Separator();
    ImGui::SliderFloat("Scale Rect1", &ScalerSlider1, 0.01f, 2.0f, "%.2f");

    ImGui::End();

    m_Scene->OnImGuiRender();
}


void TestLayer::DrawPositionSliders()
{
    auto camera = m_Scene->GetEditorCamera();
    camera->UpdateView();
    auto pos = camera->GetPos();

    static float slider_padding_x = 32.0f;
    static float reset_color_val = 1.0f; // for each color entry in RGB
    float line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 button_size = { line_height + 8.0f, line_height };
    ImVec2 window_size = ImGui::GetWindowDockNode()->Size;
    static float slider_width = window_size.x - button_size.x - slider_padding_x - 20.0f /* extra padding */;
    float slider_pos_x = ( (window_size.x * 0.50f) - (slider_width * 0.50f) );

    static ImGuiSliderFlags rectpos_slider_flags = ImGuiSliderFlags_None;
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

    ImGui::SetCursorPosX(slider_pos_x);
    ImGui::Text("Position");
    ImGui::SetCursorPosX(slider_pos_x);
    ImGui::SetNextItemWidth(slider_width);
    //ImGui::SliderFloat("##member-RectPos-x", &Translation.x, -100.0f, 100.0f, "%.2f", rectpos_slider_flags);
    ImGui::SliderFloat("##member-RectPos-x", &pos.x, -2.0f, 2.0f, "%.3f", rectpos_slider_flags);

    ImGui::SetCursorPosX(slider_pos_x);
    ImGui::SetNextItemWidth(slider_width);
    //ImGui::SliderFloat("##member-RectPos-y", &Translation.y, -100.0f, 100.0f, "%.2f", rectpos_slider_flags);
    ImGui::SliderFloat("##member-RectPos-y", &pos.y, -2.0f, 2.0f, "%.3f", rectpos_slider_flags);

    ImGui::SetCursorPosX(slider_pos_x);
    ImGui::SetNextItemWidth(slider_width);
    //ImGui::SliderFloat("##member-RectPos-z", &Translation.z, -100.0f, 100.0f, "%.2f", rectpos_slider_flags);
    ImGui::SliderFloat("##member-RectPos-z", &pos.z, -100.0f, 100.0f, "%.2f", rectpos_slider_flags);

    camera->SetPos(pos);

    ImGui::PopStyleVar(1);
}


void TestLayer::DrawColorSliders()
{
    static ImGuiSliderFlags color_slider_flags = ImGuiSliderFlags_None;
    static float slider_padding_x = 32.0f;
    static float reset_color_val = 1.0f; // for each color entry in RGB
    float line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 button_size = { line_height + 8.0f, line_height };
    ImVec2 window_size = ImGui::GetWindowDockNode()->Size;
    static float slider_width = window_size.x - button_size.x - slider_padding_x - 20.0f /* extra padding */;
    float slider_pos_x = ( (window_size.x * 0.50f) - (slider_width * 0.50f) );

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
    ImGui::SeparatorText("Rectangle Config");
    ImGui::Dummy(ImVec2(0, 40));

    //ImGui::SetCursorPosX(slider_pos_x);
    /* RED */
    ImGui::SetCursorPosX(window_size.x / 2 - ImGui::CalcTextSize("Color").x);
    ImGui::Text("Color");
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.90f, 0.0f, 0.0f, 0.90f));
    if (ImGui::Button("R", button_size))
    {
        ColorSlider.x = reset_color_val;
    }
    ImGui::PopStyleColor(2);
    //ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::SetCursorPosX(slider_pos_x);
    ImGui::SetNextItemWidth(slider_width);
    ImGui::SliderFloat("##member-ColorSlider-x", &ColorSlider.x, 0.0f, 1.0f, "%.3f", color_slider_flags);

    /* GREEN */
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.135f, 0.85f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.90f, 0.135f, 0.50f));
    if (ImGui::Button("G", button_size))
        ColorSlider.y = reset_color_val;
    ImGui::PopStyleColor(2);
    //ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::SetCursorPosX(slider_pos_x);
    ImGui::SetNextItemWidth(slider_width);
    ImGui::SliderFloat("##member-ColorSlider-y", &ColorSlider.y, 0.0f, 1.0f,"%.3f", color_slider_flags);

    /* BLUE */
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.90f, 0.90f));
    if (ImGui::Button("B", button_size))
        ColorSlider.z = reset_color_val;
    ImGui::PopStyleColor(2);
    //ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::SetCursorPosX(slider_pos_x);
    ImGui::SetNextItemWidth(slider_width);
    ImGui::SliderFloat("##member-ColorSlider-z", &ColorSlider.z, 0.0f, 1.0f, " %.3f", color_slider_flags);

    /* ALPHA */
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.70f, 0.70f, 0.70f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.70f, 0.70f, 0.70f, 0.90f));
    if (ImGui::Button("A", button_size))
        ColorSlider.z = reset_color_val;
    ImGui::PopStyleColor(2);
    //ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::SetCursorPosX(slider_pos_x);
    ImGui::SetNextItemWidth(slider_width);
    ImGui::SliderFloat("##member-ColorSlider-w", &ColorSlider.w, 0.0f, 1.0f, " %.3f", color_slider_flags);


    ImGui::PopStyleVar(1);
}
