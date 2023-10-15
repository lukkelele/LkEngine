#include "TestLayer.h"
#include <LkEngine/UI/UI.h>


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

    m_Shader = new Shader("assets/shaders/basic_color.shader");
    m_Shader->Bind();
    m_Shader->SetUniform4f("u_Color", ColorSlider.x, ColorSlider.y, ColorSlider.z, ColorSlider.w);
    m_Shader->Unbind();
}

void TestLayer::OnDetach()
{
}

void TestLayer::OnUpdate(float ts)
{
    m_Shader->Bind();
    m_Shader->SetUniform4f("u_Color", ColorSlider.x, ColorSlider.y, ColorSlider.z, ColorSlider.w);
    Renderer::Draw(*m_VAO, *m_IBO, *m_Shader);
}

void TestLayer::OnImGuiRender()
{
    ImGui::Begin(LkEngine::UI::SIDEBAR_LEFT);
    static ImGuiSliderFlags color_slider_flags = ImGuiSliderFlags_None;

    static float slider_padding_x = 32.0f;
    static float reset_color_val = 1.0f; // for each color entry in RGB
    float line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 button_size = { line_height + 8.0f, line_height };

    ImVec2 window_size = ImGui::GetWindowDockNode()->Size;
    static float slider_width = window_size.x - button_size.x - slider_padding_x - 20.0f /* extra padding */;
    float slider_pos_x = ( (window_size.x * 0.50f) - (slider_width * 0.50f) );

    //ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
    //ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
    //ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
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
        ColorSlider.x = reset_color_val;
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


    static ImGuiSliderFlags rectpos_slider_flags = ImGuiSliderFlags_None;
    ImGui::SetCursorPosX(slider_pos_x);
    ImGui::Text("Position");
    ImGui::SetCursorPosX(slider_pos_x);
    ImGui::SetNextItemWidth(slider_width);
    ImGui::SliderFloat("##member-RectPos-x", &RectPos.x, -100.0f, 100.0f, "%.2f", rectpos_slider_flags);

    ImGui::SetCursorPosX(slider_pos_x);
    ImGui::SetNextItemWidth(slider_width);
    ImGui::SliderFloat("##member-RectPos-y", &RectPos.y, -100.0f, 100.0f, "%.2f", rectpos_slider_flags);
    //ImGui::DragFloat("##member-RectPos-y", &RectPos.y, 0.01, 0.0f, 1.0f, "%.2f");

    ImGui::SetCursorPosX(slider_pos_x);
    ImGui::SetNextItemWidth(slider_width);
    ImGui::SliderFloat("##member-RectPos-z", &RectPos.z, -100.0f, 100.0f, "%.2f", rectpos_slider_flags);

    ImGui::PopStyleVar(1);
    //ImGui::EndChild();

    ImGui::End();
}
