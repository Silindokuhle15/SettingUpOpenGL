#include "Scene.h"

void Scene::AttachCamera(std::shared_ptr<Camera> cam)
{
	//m_ActiveCamera = std::make_shared<PerspectiveCamera>(*cam);
	m_ActiveCamera = cam;
}

void Scene::OnCreateSceneObjects()
{
	// CREATE LIGHTS

	PointLight pointLight;
	Vector3 dummy_pos = {};
	Vector3 dummy_color = {};

	// CREATE THE SCENE CAMERA(s)
	Camera pCam;
	std::vector<const char*> vec3_keys =
	{
		"x",
		"y",
		"z"
	};
	Vector3 vec3 = {};
	std::string str = "";
	std::vector<std::string> OBJ_paths;
	std::vector<std::string> shader_paths;
	std::vector<std::string> static_mesh_paths;
	std::vector<std::string> dynamic_mesh_paths;

	lua_Number number = 0;

	for (auto& var : m_LuaEngine.Keys)
	{
		lua_getglobal(m_LuaEngine.m_pLuaState, var.c_str());
		switch (lua_type(m_LuaEngine.m_pLuaState, -1))
		{
		case LUA_TTABLE:

			if (var == "static_geometry")
			{
				if (lua_getfield(m_LuaEngine.m_pLuaState, -1, "OBJ_path"))
				{
					str = lua_tostring(m_LuaEngine.m_pLuaState, -1);
					static_mesh_paths.push_back(str);
					lua_pop(m_LuaEngine.m_pLuaState, 1);
				}
				break;
			}
			else if (var == "dynamic_geometry")
			{
				if (lua_getfield(m_LuaEngine.m_pLuaState, -1, "OBJ_path"))
				{
					str = lua_tostring(m_LuaEngine.m_pLuaState, -1);
					dynamic_mesh_paths.push_back(str);
					lua_pop(m_LuaEngine.m_pLuaState, 1);
				}
				break;
			}
			else if (var == "material")
			{
				if (lua_getfield(m_LuaEngine.m_pLuaState, -1, "VShaderPath"))
				{
					str = lua_tostring(m_LuaEngine.m_pLuaState, -1);
					shader_paths.push_back(str);
					lua_pop(m_LuaEngine.m_pLuaState, 1);
				}
				if (lua_getfield(m_LuaEngine.m_pLuaState, -1, "FShaderPath"))
				{
					str = lua_tostring(m_LuaEngine.m_pLuaState, -1);
					shader_paths.push_back(str);
					lua_pop(m_LuaEngine.m_pLuaState, 1);
				}
				if (lua_getfield(m_LuaEngine.m_pLuaState, -1, "material_texture"))
				{
					str = lua_tostring(m_LuaEngine.m_pLuaState, -1);
					shader_paths.push_back(str);
					lua_pop(m_LuaEngine.m_pLuaState, 1);
				}
				if (lua_getfield(m_LuaEngine.m_pLuaState, -1, "mtl_path"))
				{
					str = lua_tostring(m_LuaEngine.m_pLuaState, -1);
					shader_paths.push_back(str);
					lua_pop(m_LuaEngine.m_pLuaState, 1);
				}
				break;
			}
			else if (var == "scene_camera")
			{

				if (lua_getfield(m_LuaEngine.m_pLuaState, -1, "eye"))
				{
					m_LuaEngine.GetField(m_LuaEngine.m_pLuaState, vec3_keys, &vec3);
					pCam.m_eye = glm::vec3(vec3.x, vec3.y, vec3.z);
					lua_pop(m_LuaEngine.m_pLuaState, 1);
				}
				if (lua_getfield(m_LuaEngine.m_pLuaState, -1, "center"))
				{
					m_LuaEngine.GetField(m_LuaEngine.m_pLuaState, vec3_keys, &vec3);
					pCam.m_center = glm::vec3(vec3.x, vec3.y, vec3.z);
					lua_pop(m_LuaEngine.m_pLuaState, 1);
				}
				if (lua_getfield(m_LuaEngine.m_pLuaState, -1, "up"))
				{
					m_LuaEngine.GetField(m_LuaEngine.m_pLuaState, vec3_keys, &vec3);
					pCam.m_up = glm::vec3(vec3.x, vec3.y, vec3.z);
					lua_pop(m_LuaEngine.m_pLuaState, 1);
				}
				break;
			}

			if (var == "light_position")
			{
				dummy_pos = vec3;
			}
			else if (var == "light_color")
			{
				dummy_color = vec3;
			}
			else if (var == "sky_color")
			{
				auto var = glm::vec3(vec3.x, vec3.y, vec3.z);
				m_SkyLights.push_back(SkyLight(var));
			}
			else if (var == "ground_color")
			{
				auto var = glm::vec3(vec3.x, vec3.y, vec3.z);
				m_GroundLights.push_back(GroundLight(var));
			}
			break;
		case LUA_TSTRING:
			
			if (var == "obj_path")
			{
				str = lua_tostring(m_LuaEngine.m_pLuaState, -1);
				OBJ_paths.push_back(str);
				lua_pop(m_LuaEngine.m_pLuaState, 1);
			}
			break;

		case LUA_TNUMBER:
			number = lua_tonumber(m_LuaEngine.m_pLuaState, -1);
			var == "WIDTH" ? pCam.SetWidth(number) : void();
			var == "HEIGHT" ? pCam.SetHeight(number) : void();
			lua_pop(m_LuaEngine.m_pLuaState, 1);
 			break;
		}

	}
	
	std::vector<ShaderInfo> m_shaderInfo;
	for (size_t index = 0; index < shader_paths.size(); index += 4)
	{
		m_shaderInfo = { ShaderInfo{ shader_paths[index], GL_VERTEX_SHADER },
			ShaderInfo{ shader_paths[index+1], GL_FRAGMENT_SHADER } };
		Shader temp_shader(m_shaderInfo);

		std::string& texture_path = shader_paths[index + 2];
		std::string& mtl_path = shader_paths[index + 3];
		m_Materials.push_back(
			Material(texture_path, mtl_path, m_shaderInfo));

		m_shaderInfo.clear();
	}
	for (auto& mat : m_Materials)
	{
		mat.OnInit();
	}
	m_Lights.push_back(
		PointLight(
			glm::vec3(dummy_pos.x, dummy_pos.y, dummy_pos.z),
			glm::vec3(dummy_color.x, dummy_color.y, dummy_color.z)
			)
		);

	pCam.OnInit();
	m_ActiveCamera = std::make_shared<Camera>(pCam);
	for (auto& s_mesh : static_mesh_paths)
	{
		LoadMeshData(s_mesh.c_str(), 0);
	}

	BatchStaticGeometry();
	for (auto& d_mesh : dynamic_mesh_paths)
	{
		LoadMeshData(d_mesh.c_str(), 1);
	}
}

void Scene::OnInit()
{
	m_State = SceneState::LOADING;
	m_LuaEngine.SetScriptPath(
		"C:/dev/Silindokuhle15/Spring/Assets/Projects/Lobby.lua"
	);

	m_LuaEngine.SetKeys(
		std::vector<std::string>({ 
			"WIDTH",
			"HEIGHT",
			//"dynamic_geometry",
			//"static_geometry",
			//LIGHTS
			"light_position",
			"light_color",
			"sky_color",
			"ground_color",
			//CAMERA
			"scene_camera",
			// TEXTUTES
			"material"
			}
		)
	);
	m_LuaEngine.Run();
	OnCreateSceneObjects();

	m_PhysicsEngine.OnInit();

	m_State = SceneState::RUNNING;
}

void Scene::OnUpdate(float ts)
{
	auto func = [this, ts](std::vector<Mesh> buffer)
	{
		m_ActiveCamera->OnUpdate(ts);
		for (int i = 0; i < buffer.size(); i++)
		{
			buffer[i].OnUpdate(ts);
			unsigned int model_location = m_ModelLocations[i];
			unsigned int normal_matrix_location = m_NormalMatrixLocations[i];

			glm::mat4 transform = buffer[i].m_Transform;
			glm::mat4 model_view = transform * m_ActiveCamera->GetV();

			glm::mat4 normal_matrix = glm::transpose(glm::inverse(model_view));

			glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(transform));
			glUniformMatrix4fv(normal_matrix_location, 1, GL_FALSE, glm::value_ptr(normal_matrix));
		}

		// UPDATE THE MATERIALS

		for (auto& mtl : m_Materials)
		{
			mtl.OnUpdate();
		}
	};

	switch (m_State)
	{
	case SceneState::LOADING:

		break;

	case SceneState::RUNNING:

		func(m_StaticGeometry);
		func(m_DynamicGeometry);
		func(m_MeshData);
		m_PhysicsEngine.OnUpdate(ts);
		break;

	case SceneState::PAUSED:
		break;

	case SceneState::END:
		break;

	case SceneState::STOPPED:
		break;
	}
}

void Scene::Run()
{
	//m_State = SceneState::RUNNING;
}

void Scene::OnEnd()
{
	//m_State = SceneState::END;
}

void Scene::OnPause()
{
	//m_State = SceneState::PAUSED;
}

void Scene::OnStop()
{
	//m_State = SceneState::STOPPED;
}

void Scene::OnReload()
{
	//m_State = SceneState::LOADING;
}


void Scene::Process()
{	
	auto func = [this](std::vector<Mesh> buffer)
	{
		for (int i = 0; i < buffer.size(); i++)
		{
			glGetIntegerv(GL_CURRENT_PROGRAM, &m_ActiveMaterial); // m_CurrentProgram shoubd be the currently bound Material ID
			uint32_t model_location = glGetUniformLocation(m_ActiveMaterial, "Model");
			uint32_t normal_matrix_location = glGetUniformLocation(m_ActiveMaterial, "NormalMatrix");

			uint32_t light_location = glGetUniformLocation(m_ActiveMaterial, "LightPosition");
			//int light_color_location = glGetUniformLocation(m_ActiveMaterial, "LightColor");
			//int sky_color_location = glGetUniformLocation(m_ActiveMaterial, "SkyColor");
			//int ground_color_location = glGetUniformLocation(m_ActiveMaterial, "GroundColor");
			//int factor_location = glGetUniformLocation(m_ActiveMaterial, "factor");

			m_ModelLocations.push_back(model_location);
			m_NormalMatrixLocations.push_back(normal_matrix_location);
		}
	};

	func(m_StaticGeometry);
	func(m_DynamicGeometry);
	func(m_MeshData);
}

void Scene::LoadMeshData(const char* path, int buffer)
{
	Mesh new_mesh;

	if (std::string(path) == "Grid")
	{		
		Grid d = Grid(250,250);
		d.OnInit();
		new_mesh = Mesh(d);
		new_mesh.SetTransform(glm::mat4(1.0f));
	}
	else
	{
		new_mesh = Mesh(path);
		new_mesh.OnInit();
		new_mesh.SetTransform(glm::mat4(1.0f));
		//LoadMeshData(new_mesh);
	}
	switch (buffer)
	{
	case 1: // DYANAMIC GEOMETRY
		LoadDynamicGeometry(new_mesh);
		break;
	default:
		LoadStaticGeometry(new_mesh);
	}

}

void Scene::LoadDynamicGeometry(Mesh& other)
{
	m_DynamicGeometry.push_back(other);
	NumMeshes++;
}

void Scene::BatchStaticGeometry()
{
	Mesh BatchedMesh = Mesh::Batch(m_StaticGeometry);
	m_StaticGeometry.clear();
	m_StaticGeometry.push_back(BatchedMesh);
}

Scene::Scene(std::string path)
	:
	m_Title{path},
	m_State{ SceneState::LOADING },
	NumMeshes{ 0 },
	m_ActiveMaterial{0},
	m_MeshData{},
	m_DynamicGeometry{},
	m_StaticGeometry{}
{

	//GLuint mVBONames[VBO_COUNT];
	FbxArray<SubMesh*> mSubMeshes;
	bool mHasNormal;
	bool mHasUV;
	bool mAllByControlPoint; // Save data in VBO by control point or by polygon vertex.
	m_pManager = FbxManager::Create();

	FbxIOSettings* ios = FbxIOSettings::Create(m_pManager, IOSROOT);
	m_pManager->SetIOSettings(ios);

	FbxImporter* pFbxImporter = FbxImporter::Create(m_pManager, "");

	auto filename = m_Title.c_str();

	bool import_status = pFbxImporter->Initialize(filename, -1, m_pManager->GetIOSettings());

	m_pScene = FbxScene::Create(m_pManager, "");

	if (pFbxImporter->IsFBX())
	{
		// Set the import states. By default, the import states are always set to 
		// true. The code below shows how to change these states.
		(*(m_pManager->GetIOSettings())).SetBoolProp(IMP_FBX_MATERIAL, true);
		(*(m_pManager->GetIOSettings())).SetBoolProp(IMP_FBX_TEXTURE, true);
		(*(m_pManager->GetIOSettings())).SetBoolProp(IMP_FBX_LINK, true);
		(*(m_pManager->GetIOSettings())).SetBoolProp(IMP_FBX_SHAPE, true);
		(*(m_pManager->GetIOSettings())).SetBoolProp(IMP_FBX_GOBO, true);
		(*(m_pManager->GetIOSettings())).SetBoolProp(IMP_FBX_ANIMATION, true);
		(*(m_pManager->GetIOSettings())).SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
	}
	// Import the scene
	import_status = pFbxImporter->Import(m_pScene);

	auto ConstructMesh = [&](FbxMesh* pMesh)
		{

			std::vector<glm::vec3> pos;
			std::vector<glm::vec2> tex;
			std::vector<uint32_t> id;
			std::vector<glm::vec3> norm;

			const int lPolygonCount = pMesh->GetPolygonCount();

			// Count the polygon count of each material
			FbxLayerElementArrayTemplate<int>* lMaterialIndice = NULL;
			FbxGeometryElement::EMappingMode lMaterialMappingMode = FbxGeometryElement::eNone;
			if (pMesh->GetElementMaterial())
			{
				lMaterialIndice = &pMesh->GetElementMaterial()->GetIndexArray();
				lMaterialMappingMode = pMesh->GetElementMaterial()->GetMappingMode();
				if (lMaterialIndice && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
				{
					FBX_ASSERT(lMaterialIndice->GetCount() == lPolygonCount);
					if (lMaterialIndice->GetCount() == lPolygonCount)
					{
						// Count the faces of each material
						for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
						{
							const int lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);
							if (mSubMeshes.GetCount() < lMaterialIndex + 1)
							{
								mSubMeshes.Resize(lMaterialIndex + 1);
							}
							if (mSubMeshes[lMaterialIndex] == NULL)
							{
								mSubMeshes[lMaterialIndex] = new SubMesh;
							}
							mSubMeshes[lMaterialIndex]->TriangleCount += 1;
						}

						// Make sure we have no "holes" (NULL) in the mSubMeshes table. This can happen
						// if, in the loop above, we resized the mSubMeshes by more than one slot.
						for (int i = 0; i < mSubMeshes.GetCount(); i++)
						{
							if (mSubMeshes[i] == NULL)
								mSubMeshes[i] = new SubMesh;
						}

						// Record the offset (how many vertex)
						const int lMaterialCount = mSubMeshes.GetCount();
						int lOffset = 0;
						for (int lIndex = 0; lIndex < lMaterialCount; ++lIndex)
						{
							mSubMeshes[lIndex]->IndexOffset = lOffset;
							lOffset += mSubMeshes[lIndex]->TriangleCount * 3;
							// This will be used as counter in the following procedures, reset to zero
							mSubMeshes[lIndex]->TriangleCount = 0;
						}
						FBX_ASSERT(lOffset == lPolygonCount * 3);
					}
				}
			}

			// All faces will use the same material.
			if (mSubMeshes.GetCount() == 0)
			{
				mSubMeshes.Resize(1);
				mSubMeshes[0] = new SubMesh();
			}

			// Congregate all the data of a mesh to be cached in VBOs.
			// If normal or UV is by polygon vertex, record all vertex attributes by polygon vertex.
			mHasNormal = pMesh->GetElementNormalCount() > 0;
			mHasUV = pMesh->GetElementUVCount() > 0;
			FbxGeometryElement::EMappingMode lNormalMappingMode = FbxGeometryElement::eNone;
			FbxGeometryElement::EMappingMode lUVMappingMode = FbxGeometryElement::eNone;
			if (mHasNormal)
			{
				lNormalMappingMode = pMesh->GetElementNormal(0)->GetMappingMode();
				if (lNormalMappingMode == FbxGeometryElement::eNone)
				{
					mHasNormal = false;
				}
				if (mHasNormal && lNormalMappingMode != FbxGeometryElement::eByControlPoint)
				{
					mAllByControlPoint = false;
				}
			}
			if (mHasUV)
			{
				lUVMappingMode = pMesh->GetElementUV(0)->GetMappingMode();
				if (lUVMappingMode == FbxGeometryElement::eNone)
				{
					mHasUV = false;
				}
				if (mHasUV && lUVMappingMode != FbxGeometryElement::eByControlPoint)
				{
					mAllByControlPoint = false;
				}
			}

			// Allocate the array memory, by control point or by polygon vertex.
			int lPolygonVertexCount = pMesh->GetControlPointsCount();
			if (!mAllByControlPoint)
			{
				lPolygonVertexCount = lPolygonCount * TRIANGLE_VERTEX_COUNT;
			}
			float *lVertices = new float[lPolygonVertexCount * VERTEX_STRIDE];
			unsigned int *lIndices = new unsigned int[lPolygonCount * TRIANGLE_VERTEX_COUNT];
			float* lNormals = NULL;
			if (mHasNormal)
			{
				lNormals = new float[lPolygonVertexCount * NORMAL_STRIDE];
			}
			float* lUVs = NULL;
			FbxStringList lUVNames;
			pMesh->GetUVSetNames(lUVNames);
			const char* lUVName = NULL;
			if (mHasUV && lUVNames.GetCount())
			{
				lUVs = new float[lPolygonVertexCount * UV_STRIDE];
				lUVName = lUVNames[0];
			}

			// Populate the array with vertex attribute, if by control point.
			const FbxVector4* lControlPoints = pMesh->GetControlPoints();
			FbxVector4 lCurrentVertex;
			FbxVector4 lCurrentNormal;
			FbxVector2 lCurrentUV;
			if (mAllByControlPoint)
			{
				const FbxGeometryElementNormal* lNormalElement = NULL;
				const FbxGeometryElementUV* lUVElement = NULL;
				if (mHasNormal)
				{
					lNormalElement = pMesh->GetElementNormal(0);
				}
				if (mHasUV)
				{
					lUVElement = pMesh->GetElementUV(0);
				}
				for (int lIndex = 0; lIndex < lPolygonVertexCount; ++lIndex)
				{
					// Save the vertex position.
					lCurrentVertex = lControlPoints[lIndex];
					lVertices[lIndex * VERTEX_STRIDE] = static_cast<float>(lCurrentVertex[0]);
					lVertices[lIndex * VERTEX_STRIDE + 1] = static_cast<float>(lCurrentVertex[1]);
					lVertices[lIndex * VERTEX_STRIDE + 2] = static_cast<float>(lCurrentVertex[2]);
					lVertices[lIndex * VERTEX_STRIDE + 3] = 1;

					// Save the normal.
					if (mHasNormal)
					{
						int lNormalIndex = lIndex;
						if (lNormalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						{
							lNormalIndex = lNormalElement->GetIndexArray().GetAt(lIndex);
						}
						lCurrentNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
						lNormals[lIndex * NORMAL_STRIDE] = static_cast<float>(lCurrentNormal[0]);
						lNormals[lIndex * NORMAL_STRIDE + 1] = static_cast<float>(lCurrentNormal[1]);
						lNormals[lIndex * NORMAL_STRIDE + 2] = static_cast<float>(lCurrentNormal[2]);

					}

					// Save the UV.
					if (mHasUV)
					{
						int lUVIndex = lIndex;
						if (lUVElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						{
							lUVIndex = lUVElement->GetIndexArray().GetAt(lIndex);
						}
						lCurrentUV = lUVElement->GetDirectArray().GetAt(lUVIndex);
						lUVs[lIndex * UV_STRIDE] = static_cast<float>(lCurrentUV[0]);
						lUVs[lIndex * UV_STRIDE + 1] = static_cast<float>(lCurrentUV[1]);


					}
				}

			}

			int lVertexCount = 0;
			for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
			{
				// The material for current face.
				int lMaterialIndex = 0;
				if (lMaterialIndice && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
				{
					lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);
				}

				// Where should I save the vertex attribute index, according to the material
				const int lIndexOffset = mSubMeshes[lMaterialIndex]->IndexOffset +
					mSubMeshes[lMaterialIndex]->TriangleCount * 3;
				for (int lVerticeIndex = 0; lVerticeIndex < TRIANGLE_VERTEX_COUNT; ++lVerticeIndex)
				{
					const int lControlPointIndex = pMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);
					// If the lControlPointIndex is -1, we probably have a corrupted mesh data. At this point,
					// it is not guaranteed that the cache will work as expected.
					if (lControlPointIndex >= 0)
					{
						if (mAllByControlPoint)
						{
							lIndices[lIndexOffset + lVerticeIndex] = static_cast<unsigned int>(lControlPointIndex);
						}
						// Populate the array with vertex attribute, if by polygon vertex.
						else
						{
							lIndices[lIndexOffset + lVerticeIndex] = static_cast<unsigned int>(lVertexCount);

							lCurrentVertex = lControlPoints[lControlPointIndex];
							lVertices[lVertexCount * VERTEX_STRIDE] = static_cast<float>(lCurrentVertex[0]);
							lVertices[lVertexCount * VERTEX_STRIDE + 1] = static_cast<float>(lCurrentVertex[1]);
							lVertices[lVertexCount * VERTEX_STRIDE + 2] = static_cast<float>(lCurrentVertex[2]);
							lVertices[lVertexCount * VERTEX_STRIDE + 3] = 1;

							auto x = static_cast<float>(lCurrentVertex[0]);
							auto y = static_cast<float>(lCurrentVertex[1]);
							auto z = static_cast<float>(lCurrentVertex[2]);
							pos.push_back(glm::vec3(x, y, z));
							id.push_back(lMaterialIndex);

							if (mHasNormal)
							{
								pMesh->GetPolygonVertexNormal(lPolygonIndex, lVerticeIndex, lCurrentNormal);
								lNormals[lVertexCount * NORMAL_STRIDE] = static_cast<float>(lCurrentNormal[0]);
								lNormals[lVertexCount * NORMAL_STRIDE + 1] = static_cast<float>(lCurrentNormal[1]);
								lNormals[lVertexCount * NORMAL_STRIDE + 2] = static_cast<float>(lCurrentNormal[2]);

								auto x = static_cast<float>(lCurrentNormal[0]);
								auto y = static_cast<float>(lCurrentNormal[1]);
								auto z = static_cast<float>(lCurrentNormal[2]);
								norm.push_back(glm::vec3(x, y, z));
							}

							if (mHasUV)
							{
								bool lUnmappedUV;
								pMesh->GetPolygonVertexUV(lPolygonIndex, lVerticeIndex, lUVName, lCurrentUV, lUnmappedUV);
								lUVs[lVertexCount * UV_STRIDE] = static_cast<float>(lCurrentUV[0]);
								lUVs[lVertexCount * UV_STRIDE + 1] = static_cast<float>(lCurrentUV[1]);

								auto u = static_cast<float>(lCurrentUV[0]);
								auto v = static_cast<float>(lCurrentUV[1]);
								tex.push_back(glm::vec2(u, v));
							}
						}
					}
					++lVertexCount;
				}
				mSubMeshes[lMaterialIndex]->TriangleCount += 1;
			}

			std::vector<Vertex> v;

			for (auto index = 0; index < lPolygonVertexCount; index++)
			{
				v.push_back(Vertex{ pos[index], tex[index], id[index], norm[index] });
			}

			/*
			if (lVertices)
			{
				delete[] lVertices;
			}

			if (lIndices)
			{
				delete[] lIndices;
			}*/

			return v;
		};

	// RETRIEVE ANIMATION DATA
	int i = 0;
	//int numStacks = m_pScene->GetSrcObjectCount< FbxAnimStack>();
	//for (auto anim_index = 0; anim_index < numStacks; anim_index++)
	{
		//auto anim_stack = m_pScene->GetSrcObject< FbxAnimStack>(anim_index);
		//auto num_layers = anim_stack->GetMemberCount<FbxAnimLayer>();
		//for (auto layer_index = 0; layer_index < num_layers; layer_index++)
		{
		//	auto anim_layer = anim_stack->GetMember<FbxAnimLayer>(layer_index);
		}

	}


	FbxNodeAttribute::EType lAttributeType;

	auto node_count = m_pScene->GetNodeCount();
	//for (auto node_index = 0; node_index < 1; node_index++)
	for (auto node_index = 1; node_index < m_pScene->GetNodeCount(); node_index++)
	{
		auto child_node = m_pScene->GetNode(node_index);
		auto node_attrib = child_node->GetNodeAttribute();
		auto node_attrib_type = node_attrib->GetAttributeType();

		auto _scl = child_node->LclScaling.Get();
		auto _rot = child_node->LclRotation.Get();
		auto _pos = child_node->LclTranslation.Get();

		glm::vec3 scl = glm::vec3(_scl[0] * 0.001, _scl[1] * 0.001, _scl[2] * 0.001);
		glm::vec3 rot = glm::vec3(_rot[0] * 0.001, _rot[1] * 0.001, _rot[2] * 0.001);
		glm::vec3 pos = glm::vec3(_pos[0] * 0.001, _pos[1] * 0.001, _pos[2]* 0.001);

		auto transform = glm::scale(glm::mat4(1.0f), scl);
		transform = glm::rotate(transform, glm::radians(90.0f), rot);
		transform = glm::translate(transform, pos);
		auto _transform = child_node->EvaluateGlobalTransform();
;

		std::cout << "name:"<< child_node->GetName() << std::endl;
		std::cout << "node type: " << node_attrib_type << std::endl;
		std::cout << "scale:" << _scl.Buffer() << std::endl;
		std::cout << "rotation:" << _rot.Buffer() << std::endl;
		std::cout << "translation:" << _pos.Buffer() << std::endl;
		std::cout << "transform:" << _transform.Buffer() << std::endl;

		FbxLight* light = nullptr;
		switch (node_attrib_type)
		{
			case FbxNodeAttribute::EType::eCamera:
				break;

			case FbxNodeAttribute::EType::eLight:
				light = reinterpret_cast<FbxLight*>(child_node->GetLight());
				break;

			case FbxNodeAttribute::EType::eMesh:
				auto mesh = reinterpret_cast<FbxMesh*>(child_node->GetGeometry());
				auto m_v = ConstructMesh(mesh);
				m_MeshData.push_back(Mesh(m_v, transform));
				break;
		}
	}
	pFbxImporter->Destroy();
}

void Scene::LoadStaticGeometry(Mesh& other)
{
	m_StaticGeometry.push_back(other);
	NumMeshes++;
}
