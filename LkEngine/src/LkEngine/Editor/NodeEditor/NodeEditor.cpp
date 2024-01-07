#include "LKpch.h"
#include "NodeEditor.h"


namespace LkEngine {

    NodeEditor::NodeEditor(std::string_view name)
        : m_Name(name)
    {
        m_NodeEditorContext = new NodeEditorContext();
        m_NodeEditorContext->Init();
    }

    NodeEditor::~NodeEditor()
    {
    }

    void NodeEditor::OnRender()
    {
    }

    void NodeEditor::OnImGuiRender()
    {
        static const int NodePin = 1;
        static const int InputPin = 1;
        static const int OutputPin = 2;

        NE::Begin("My Node Editor");
        {
            NE::BeginNode(NodePin);

            ImGui::Text("Node Editor content");

            NE::BeginPin(InputPin, NE::PinKind::Input);
            ImGui::Text("In -->");
            NE::EndPin();

            NE::BeginPin(OutputPin, NE::PinKind::Output);
            ImGui::Text("Out -->");
            NE::EndPin();

            NE::EndNode();
        }
        NE::End();
    }

    void NodeEditor::Destroy()
    {
        Save(); // Save before deletion begins
    }

    // Serialize
    void NodeEditor::Save()
    {
    }

}