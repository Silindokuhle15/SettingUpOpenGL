#pragma once
#include "GL/glew.h"
#define GLFW_INCLUDE_NONE
#include "imgui.h"
#include "imgui_internal.h"
#include "ImGuizmo.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <string>
#include "Scene.h"
#include <set>


#include <filesystem>

static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);

class Layer
{

public:

	Layer() = default;
	~Layer() = default;

	virtual void Enable() = 0;
	virtual void OnInit() = 0;
	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;
	virtual void OnUpdate(float ts) = 0;
};

class Panel
{
public:

	virtual void Run() = 0;
	Panel() = default;
	~Panel() = default;


private:
	std::string m_PanelName;
};


template<class T>
class ComponentPanel : public Panel
{
public:

	// SETTING VARIABLES
	bool m_EnableLighting = false;
	bool m_EnableTexture = 0;
	bool m_IsEnable = false;
	// IMGUIZMO
	bool m_UseGizmoWindow = false;
	bool m_EnableImGuizmo = false;

	// Huh?
	float m_CameraSpeed;
	std::shared_ptr<Scene> m_ActiveScene;
	int m_ActiveMaterial;
	int m_ActiveUniforms;

	// EDITOR CAMERA VARIABLES
	float m_CameraPosition[3] = { 0.0, 0.0, 0.0 };
	int m_NumberOfCamera = 1;

	// LIGHT 
	float m_Factor = 0.0;
	float m_GroundColor[3] = { 0.0, 0.0, 0.0 };
	float m_LightColor[3] = { 1.0f, 1.0f, 1.0f };
	float m_SkyColor[3] = { 0.0, 0.0, 0.0 };
	float m_PointLightPosition[3] = { 0.0, 0.0, 0.0 };

	// OBJECT POINTERS
	std::shared_ptr<PerspectiveCamera> m_EditorCamera;
	std::shared_ptr<glm::mat4> m_ActiveTransform;

	void EditTransform(float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition);

	void Run() override;

	ComponentPanel():
		m_PanelName{"Components Panel"}
	{

	}

	~ComponentPanel()
	{

	}

private:
	std::string m_PanelName;
};

template<class T>
class ContentBrowser : public Panel
{
public:

	void ReadDirTree(const std::filesystem::path& pathToShow, int level);

	void DisplayDirTree(const std::filesystem::path& pathToShow, int level);

	void Run() override;

	ContentBrowser() :
		m_PanelName{ "Content Browser" }
	{

	}

	~ContentBrowser()
	{

	}

private:

	std::filesystem::path root_path;
	std::filesystem::path current_path;

	std::string filename;
	std::string stem;
	std::set<std::string> m_DirContent;
	std::set<std::filesystem::path> m_DirEntries;
	std::string m_PanelName;
};


template<class T>
class RenderPanel : public Panel
{
public:

	void Run() override;
	RenderPanel() :
		m_PanelName{"Render Panel"}
	{
		// DO something;
	}

	~RenderPanel()
	{

	}

private:
	std::string m_PanelName;
};


template<class T>
class StatsPanel : public Panel
{
public:

	// hmm?
	bool m_RenderMode;
	unsigned int m_NumPrimitives;
	unsigned int m_NumIndices;

	// Queries
	unsigned int m_Query;
	int m_Samples;
	int m_ResultAvailable;

	void Run() override;
	StatsPanel() : 
		m_PanelName{"Stats Panel"}
	{
	}

	~StatsPanel()
	{

	}

private:
	std::string m_PanelName;
};

template<class T>
inline void ComponentPanel<T>::EditTransform(float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition)
{
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
	static bool useSnap = false;
	static float snap[3] = { 1.f, 1.f, 1.f };
	static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
	static bool boundSizing = false;
	static bool boundSizingSnap = false;


		if (ImGui::IsKeyPressed(ImGuiKey_T))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(ImGuiKey_E))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		if (ImGui::RadioButton("Universal", mCurrentGizmoOperation == ImGuizmo::UNIVERSAL))
			mCurrentGizmoOperation = ImGuizmo::UNIVERSAL;
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
		ImGui::InputFloat3("Tr", matrixTranslation);
		ImGui::InputFloat3("Rt", matrixRotation);
		ImGui::InputFloat3("Sc", matrixScale);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

		if (mCurrentGizmoOperation != ImGuizmo::SCALE)
		{
			if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
				mCurrentGizmoMode = ImGuizmo::LOCAL;
			ImGui::SameLine();
			if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
				mCurrentGizmoMode = ImGuizmo::WORLD;
		}
		if (ImGui::IsKeyPressed(ImGuiKey_S))
			useSnap = !useSnap;
		ImGui::Checkbox("##UseSnap", &useSnap);
		ImGui::SameLine();

		switch (mCurrentGizmoOperation)
		{
		case ImGuizmo::TRANSLATE:
			ImGui::InputFloat3("Snap", &snap[0]);
			break;
		case ImGuizmo::ROTATE:
			ImGui::InputFloat("Angle Snap", &snap[0]);
			break;
		case ImGuizmo::SCALE:
			ImGui::InputFloat("Scale Snap", &snap[0]);
			break;
		}
		ImGui::Checkbox("Bound Sizing", &boundSizing);
		if (boundSizing)
		{
			ImGui::PushID(3);
			ImGui::Checkbox("##BoundSizing", &boundSizingSnap);
			ImGui::SameLine();
			ImGui::InputFloat3("Snap", boundsSnap);
			ImGui::PopID();
		}

	ImGuiIO& io = ImGui::GetIO();
	float viewManipulateRight = io.DisplaySize.x;
	float viewManipulateTop = 0;
	static ImGuiWindowFlags gizmoWindowFlags = 0;
	if (m_UseGizmoWindow)
	{
		ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_Appearing);
		ImGui::SetNextWindowPos(ImVec2(400, 20), ImGuiCond_Appearing);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor(0.35f, 0.3f, 0.3f));
		ImGui::Begin("Gizmo", 0, gizmoWindowFlags);
		ImGuizmo::SetDrawlist();
		float windowWidth = (float)ImGui::GetWindowWidth();
		float windowHeight = (float)ImGui::GetWindowHeight();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
		viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
		viewManipulateTop = ImGui::GetWindowPos().y;
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		gizmoWindowFlags = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max) ? ImGuiWindowFlags_NoMove : 0;
	}
	else
	{
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	}

	//ImGuizmo::DrawGrid(cameraView, cameraProjection, identityMatrix, 100.f);
	//ImGuizmo::DrawCubes(cameraView, cameraProjection, &objectMatrix[0][0], gizmoCount);
	//ImGuizmo::DrawCubes(cameraView, cameraProjection, (float*)glm::value_ptr(*m_ActiveTransform), gizmoCount);
	ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

	glm::vec3 displacement = m_EditorCamera->GetPosition() - glm::vec3(0);
	float distance = glm::dot(displacement, displacement);
	distance = glm::sqrt(distance);

	ImGuizmo::ViewManipulate(cameraView, distance, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);

	if (m_UseGizmoWindow)
	{
		ImGui::End();
		ImGui::PopStyleColor(1);
	}
}

template<class T>
inline void ComponentPanel<T>::Run()
{
	glGetIntegerv(GL_CURRENT_PROGRAM, &m_ActiveMaterial); // m_CurrentProgram shoubd be the currently bound Material ID
	glGetProgramiv(m_ActiveMaterial, GL_ACTIVE_UNIFORMS, &m_ActiveUniforms);

	ImGui::Begin(m_PanelName.c_str());
	ImGui::Separator();
	ImGui::Checkbox("Enable ImGuizmo", &m_EnableImGuizmo);
	ImGuizmo::Enable(m_EnableImGuizmo);
	ImGui::Text("Viewing and Transformation");
	ImGui::Separator();

	ImGui::Text("Number Of Cameras : %d", m_NumberOfCamera);
	//ImGui::SliderFloat3("Camera Position", m_CameraPosition, -50.0, 50.0);
	ImGui::SliderFloat3("Camera Position", (float*)glm::value_ptr(m_EditorCamera->GetPosition()), -50.0, 50.0);
	ImGui::SliderFloat("Camera Speed", (float*)&m_EditorCamera->m_Speed, 0.0, 1.0f, "%.2f", 0);
	ImGui::Separator();

	ImGui::Text("Lights and Shadow");

	int enable_lighting_locatin = glGetUniformLocation(m_ActiveMaterial, "EnableTexture");
	glProgramUniform1i(m_ActiveMaterial, enable_lighting_locatin, m_EnableLighting);

	ImGui::Checkbox("Enable Lighting", &m_EnableLighting);
	ImGui::Text("HemiSphere Lighting Model");
	ImGui::SliderFloat3("Sky Color", m_SkyColor, 0.0, 1.0);
	ImGui::SliderFloat3("Grounr Color", m_GroundColor, 0.0, 1.0);
	int light_location = glGetUniformLocation(m_ActiveMaterial, "LightPosition");
	int light_color_location = glGetUniformLocation(m_ActiveMaterial, "LightColor");
	int sky_color_location = glGetUniformLocation(m_ActiveMaterial, "SkyColor");
	int ground_color_location = glGetUniformLocation(m_ActiveMaterial, "GroundColor");
	int factor_location = glGetUniformLocation(m_ActiveMaterial, "factor");

	glProgramUniform3fv(m_ActiveMaterial, light_location, 1, m_PointLightPosition);
	glProgramUniform3fv(m_ActiveMaterial, light_color_location, 1, m_LightColor);
	glProgramUniform3fv(m_ActiveMaterial, sky_color_location, 1, m_SkyColor);
	glProgramUniform3fv(m_ActiveMaterial, ground_color_location, 1, m_GroundColor);
	glProgramUniform3fv(m_ActiveMaterial, factor_location, 1, &m_Factor);

	ImGui::SliderFloat3("Light Position", m_PointLightPosition, -50.0, 50.0);
	ImGui::SliderFloat3("Light Color", m_LightColor, 0.0, 1.0);
	ImGui::SliderFloat("Factor [a]", &m_Factor, 0.0, 1.0);
	ImGui::Separator();

	ImGui::Text("Materials");
	ImGui::Separator();

	ImGui::Text("Active Material : %d", m_ActiveMaterial);

	int enable_texture_location = glGetUniformLocation(m_ActiveMaterial, "EnableTexture");

	glProgramUniform1i(m_ActiveMaterial, enable_texture_location, m_EnableTexture);

	ImGui::Checkbox("Apply Texture", &m_EnableTexture);
	int attached_shaders = 0;
	ImGui::Text("Attached Shaders : % d", attached_shaders);
	int tex_slot = 0;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &tex_slot);
	ImGui::Text("Active Texture Slot : %d", 0);
	ImGui::Text("Number of Active Uniforms : %d", m_ActiveUniforms);

	int width = 0, height = 0, size = 0;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
	//glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_BUFFER_SIZE, &size);
	ImGui::Text("Active Texture Dims : [%d , %d]", width, height);
	//ImGui::Text("Active Texture Size : [ %d ]", size);

	//ImGui::Checkbox("use Gizmo Window", &m_UseGizmoWindow);
	//ImGui::End();
	//ImGuizmo::SetOrthographic(false);
	//ImGuizmo::BeginFrame(); // ImGuizmo Begin Frame

	ImGui::Separator();
	ImGui::Text("Edit Transform Component");

	EditTransform((float*)glm::value_ptr(m_EditorCamera->GetV()), (float*)glm::value_ptr(m_EditorCamera->GetP()), (float*)glm::value_ptr(*m_ActiveTransform), true);

	ImGui::End();
}


template<class T>
inline void ContentBrowser<T>::DisplayDirTree(const std::filesystem::path& pathToShow, int level)
{
	// GET THE PARENT WINDOW SIZE
	auto current_window_rect = ImGui::GetCurrentWindow()->Rect();
	//auto current_window_rect = ImGui::GetCurrentContext()->CurrentWindow->InnerRect();

	float x = current_window_rect.GetWidth();
	float y = current_window_rect.GetHeight();

	float a = 0.4;
	float b = 0.9;

	//root_path = pathToShow;
	current_path = pathToShow;

	ReadDirTree(current_path, level);

	if (ImGui::BeginChild("FIrst Window", ImVec2( x * a, b * y ), true))
	{
		ImGui::Text(pathToShow.string().c_str());

		for (auto& i : m_DirEntries)
		{
			std::string dir_stem;

			if (i.has_stem())
			{
				dir_stem = i.stem().string();
			}

			if (ImGui::TreeNode(dir_stem.c_str()))
			{

				if (ImGui::IsItemClicked() && ImGui::IsItemHovered())
				{
					m_DirContent.clear();
					m_DirEntries.clear();

					current_path = i;
					current_path /= dir_stem;
					root_path = current_path;

					//ImGui::TreePush();
				}
				ImGui::TreePop();
			}
		}
	}
	ImGui::EndChild();

	ImGui::SameLine();

	if (ImGui::BeginChild("Second Window", ImVec2((x * (1 - 1.09 * a )), b * y), true))
	{
		ImGui::Text(current_path.string().c_str());


		for (auto& i : m_DirContent)
		{
			ImGui::Text(i.c_str());
		}
	}
	ImGui::EndChild();

}

template<class T>
inline void ContentBrowser<T>::ReadDirTree(const std::filesystem::path& pathToShow, int level)
{
	if (std::filesystem::exists(pathToShow) && std::filesystem::is_directory(pathToShow))
	{
		for (const auto& entry : std::filesystem::directory_iterator(pathToShow))
		{
			//auto filename = entry.path().filename();
			filename = entry.path().string();
			stem = entry.path().stem().string();

			// MAKE THE PATH RELATIVE

			if (std::filesystem::is_directory(entry.status()))
			{
				// DIRECTORY LEVEL
				m_DirEntries.insert(entry.path());
			}
			else if (std::filesystem::is_regular_file(entry.status()))
			{
				if (entry.path().has_extension())
				{
					auto name = stem + entry.path().extension().string();
					m_DirContent.insert(name.c_str());
				}
				else
				{
					m_DirContent.insert(stem.c_str());
				}
			}
		}
	}
}

template<class T>
inline void ContentBrowser<T>::Run()
{

	ImGui::Begin(m_PanelName.c_str());

	std::string dummy_path = "C:/dev/Silindokuhle15/Spring/Assets/";
	root_path =  std::filesystem::path(dummy_path);
	DisplayDirTree(root_path, 0);

	ImGui::End();
}


template<class T>
inline void RenderPanel<T>::Run()
{
	ImGui::Begin(m_PanelName.c_str());


	ImGui::End();

}

template<class T>
inline void StatsPanel<T>::Run()
{
	ImGui::Begin(m_PanelName.c_str());
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Separator();
	ImGui::Text("Active Render Mode");
	ImGui::Checkbox("Wire Frame", &m_RenderMode);
	ImGui::Separator();
	ImGui::Text("Draw Calls : %d", 1);
	ImGui::Separator();
	//ImGui::SliderInt("Scene Number ", &, 0, 4);
	ImGui::End();

	m_ResultAvailable = false;
	
	glGetQueryObjectiv(m_Query, GL_QUERY_RESULT_AVAILABLE, &m_ResultAvailable);
	if (m_ResultAvailable)
	{
		glGetQueryObjectiv(m_Query, GL_QUERY_RESULT, &m_Samples);
	}
}


