#include "TestLayer.h"
#include <iostream>


TestLayer::TestLayer()
{
    m_Name = "TestLayer";
}

TestLayer::~TestLayer()
{
}

void TestLayer::OnAttach()
{
    using namespace LkEngine;
    LOG_DEBUG("TestLayer::OnAttach()");
}

void TestLayer::OnDetach()
{
}

void TestLayer::OnImGuiRender()
{
    ImGui::Text("TestLayer::OnImGuiRender()");
}
