#include "Teaport.h"

void Teaport::OnInit()
{
    ObjectLoader ob;
    ob.LoadObjectFromFile("Objects/Uta_teaport.obj");

    m_Positions = ob.m_Positions;
    m_TexCoords = ob.m_TexCoords;
    m_VertexIndices = ob.m_VertexIndices;

    m_Positions.shrink_to_fit();
    m_VertexIndices.shrink_to_fit();

    Base::m_IndexCount = m_VertexIndices.size();
}