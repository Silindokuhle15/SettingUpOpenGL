#include "Square.h"


void Square::OnInit()
{
    m_VAO.OnInit();
    m_VAO.Bind();

    m_Positions = { {-1.0f, -1.0f, 0.0f}, { +1.0f, -1.0f, 0.0f }, {+1.0f, +1.0f, 0.0f}, { -1.0f, +1.0f, 0.0f} };
    m_TexCoords = { { 0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, { 0.0f, 1.0f} };
    m_VertexIndices = { 0, 1 , 2, 2, 3, 0};

    m_Positions.shrink_to_fit();
    m_VertexIndices.shrink_to_fit();
    
    std::vector<VertexAttrib> attribs;
    attribs.push_back(VertexAttrib::Position);
    attribs.push_back(VertexAttrib::TexCoord);
    m_VAO.CreateVertexArrayLayout(m_Shader.GetShaderProgram(), attribs);

    Base::m_IndexCount = m_VertexIndices.size();   
}