#include "LKpch.h"
#include "NodeEditor.h"


namespace LkEngine {

    NodeEditor::NodeEditor()
    {
        m_NodeEditorContext = new NodeEditorContext();
    }

    NodeEditor::~NodeEditor()
    {
    }

    void NodeEditor::Init()
    {
        m_NodeEditorContext->Init();
        NE::SetCurrentEditor(m_NodeEditorContext->GetContext());
        m_Initialized = true;
    }

    void NodeEditor::OnRender()
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

}