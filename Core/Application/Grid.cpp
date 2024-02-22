#include "Grid.h"

void Grid::OnInit()
{
    float x_offset = float(m_Columns)/2;
    float y_offset = 0;
    float z_offset = 0;

    float z = 0;

    auto ConstructTriangles = [](std::vector<glm::vec3> pos, std::vector<glm::vec2> tex, std::vector<glm::vec3> norm, uint32_t index)
        {
            return Vertex{ pos[index], tex[index], index + 10, norm[index] };
        };

    for (int j = 0; j < m_Columns; j++)
    {
        for (int i = 0; i < m_Rows; i++)
        {
            uint32_t id = i + j * i;
            std::vector<Vertex> v1 = {
                {{i - x_offset, j + y_offset, z + z_offset}, {i / m_Columns, j / m_Rows}, id, {0,0,1}},
                {{i + 1 - x_offset, j + y_offset, z + z_offset}, {(i + 1) / m_Columns, j / m_Rows}, id, {0, 0, 1} },
                {{i + 1 - x_offset, j + 1 + y_offset, z + z_offset}, {(i + 1) / m_Columns, (j + 1) / m_Rows}, id,  {0,0,1}}
            };

            std::vector<Vertex> v2 = {
                {{i - x_offset, j + y_offset, z + z_offset}, {i / m_Columns , j / m_Rows},  id, {0,0,1}},
                {{i + 1 - x_offset, j + 1 + y_offset, z + z_offset}, {(i + 1) / m_Columns, (j + 1) / m_Rows}, id, {0, 0, 1}},
                {{i - x_offset, j + 1 + y_offset, z + z_offset}, {i / m_Columns, (j + 1) / m_Rows}, id, {0,0,1}}
            };
            std::vector<uint32_t> v_indices1 =
            {
                0, 1, 2
            };

            std::vector<uint32_t> v_indices2 =
            {
                0, 2, 3
            };

            for (auto& i : v_indices1) {
                m_VertexIndices.push_back(i);
            }
            for (auto& i : v_indices2) {
                m_VertexIndices.push_back(i);
            }

            for (auto& v : v1)
            {
                m_V.push_back(v);
                m_Positions.push_back(v.pos);
                m_TexCoords.push_back(v.tex);
                m_VertexIDs.push_back(v.ID);
                m_Normals.push_back(v.norm);
            }

            for (auto& v : v2)
            {
                m_V.push_back(v);
                m_Positions.push_back(v.pos);
                m_TexCoords.push_back(v.tex);
                m_VertexIDs.push_back(v.ID);
                m_Normals.push_back(v.norm);
            }

        }

        NumIndices = m_VertexIndices.size();
        NumVertices = m_Positions.size();
    }

}
