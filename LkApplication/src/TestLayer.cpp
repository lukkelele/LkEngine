#include "TestLayer.h"


TestLayer::TestLayer()
{
    m_Name = "TestLayer";
}

TestLayer::~TestLayer()
{
}

void TestLayer::OnAttach()
{
    // using namespace LkEngine;
    LOG_DEBUG("TestLayer::OnAttach()");
}

void TestLayer::OnDetach()
{
}

void TestLayer::OnImGuiRender()
{
    // ImGui::Begin("TestLayer");
    // ImGui::Text("TestLayer::OnImGuiRender()");
    // ImGui::End();
}
