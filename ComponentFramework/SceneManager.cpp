#include <SDL.h>
#include "SceneManager.h"
#include "Timer.h"
#include "Scene0.h"
#include <thread>

#include "OpenGLRenderer.h"
#include "VulkanRenderer.h"

#include "ControllerManager.h"
#include "FunctionProfiler.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

// This style using : to initialize
// gets them done before the constructor even starts
SceneManager::SceneManager(): 
	currentScene(nullptr), timer(nullptr), controllerManager(nullptr),
	fps(60), isRunning(false), fullScreen(false), show_demo_window(false),
	handleEventsProfiler(nullptr), updateProfiler(nullptr), renderProfiler(nullptr),
	rendererType(RendererType::OPENGL)
{
	Debug::Info("Starting the SceneManager", __FILE__, __LINE__);
}

SceneManager::~SceneManager() {
	if (currentScene) {
		currentScene->OnDestroy();
		delete currentScene;
		currentScene = nullptr;
	}
	
	if (timer) {
		delete timer;
		timer = nullptr;
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	Debug::Info("Deleting the SceneManager", __FILE__, __LINE__);
}

bool SceneManager::Initialize(std::string name_, int width_, int height_) {

	switch (rendererType)
	{
	case RendererType::OPENGL:
		renderer = std::make_shared<OpenGLRenderer>();
		renderer->SetRendererType(RendererType::OPENGL);
		renderer->NewWindow(name_, width_, height_);
		renderer->OnCreate();

		//window = new Window();
		//if (!window->OnCreate(name_, width_, height_)) {
		//	Debug::FatalError("Failed to initialize Window object", __FILE__, __LINE__);
		//	return false;
		//}
		break;
	case RendererType::VULKAN:
		// TODO:: Implement Vulkan Renderer
		//renderer = std::make_shared<VulkanRenderer>();
		//renderer->SetRendererType(RendererType::VULKAN);

		break;
	case RendererType::DIRECTX11:
		Debug::FatalError("DirectX11 not supported", __FILE__, __LINE__);
		throw;
		break;
	case RendererType::DIRECTX12:
		Debug::FatalError("DirectX12 not supported", __FILE__, __LINE__);
		throw;
		break;
	case RendererType::NONE:
		Debug::FatalError("No Renderer Selected", __FILE__, __LINE__);
		throw;
		break;
	default:
		break;
	}

	timer = new Timer();
	if (timer == nullptr) {
		Debug::FatalError("Failed to initialize Timer object", __FILE__, __LINE__);
		return false;
	}

	controllerManager = std::make_shared<ControllerManager>();
	if (!controllerManager->OnCreate()) {
		Debug::FatalError("Failed to initialize Controller Manager", __FILE__, __LINE__);
		return false;
	}

	updateProfiler = std::make_shared<Profiler>();
	if (updateProfiler == nullptr)
	{
		Debug::FatalError("Failed to initialize Update Profiler", __FILE__, __LINE__);
		return false;
	}
	handleEventsProfiler = std::make_shared<Profiler>();
	if (handleEventsProfiler == nullptr)
	{
		Debug::FatalError("Failed to initialize Handle Events Profiler", __FILE__, __LINE__);
		return false;
	}
	renderProfiler = std::make_shared<Profiler>();
	if(renderProfiler == nullptr)
	{
		Debug::FatalError("Failed to initialize Render Profiler", __FILE__, __LINE__);
		return false;
	}


	/********************************   Default first scene   ***********************/
	BuildNewScene(SCENE_NUMBER::SCENE0);

	// ImGui Setup
	switch (rendererType)
	{
	case RendererType::OPENGL:
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		glRenderer = std::dynamic_pointer_cast<OpenGLRenderer>(renderer);
		ImGui_ImplSDL2_InitForOpenGL(glRenderer->GetWindow(), glRenderer->GetContext());
		ImGui_ImplOpenGL3_Init("#version 450");
		io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\Arial.ttf", 18.0f);
		break;
	}
	case RendererType::VULKAN:
		// TODO:: Implement Vulkan Renderer ImGui
		break;
	default:
		break;
	}

	
	return true;
}

/// This is the whole game
void SceneManager::Run() {
	timer->Start();
	isRunning = true;

	while (isRunning) {

		// Handle Events
		HandleEvents();
		//std::thread eventsThread([this] {HandleEvents(); });
	
		// Update
		Update();
		//std::thread updateThread([this] {Update(); });

		// Render
		Render();
		//std::thread renderThread([this] {Render(); });

		//eventsThread.join();
		//updateThread.join();
		//renderThread.join();

		HandleGUI();

		switch (rendererType)
		{
		case RendererType::OPENGL:
			SDL_GL_SwapWindow(glRenderer->GetWindow());
			SDL_Delay(timer->GetSleepTime(fps));
			break;
		case RendererType::VULKAN:
			// TODO:: Implement Vulkan Renderer ImGui
			break;
		default:
			break;
		}
	}
}

void SceneManager::HandleEvents() {

	if (enableProfilers)
	{
		handleEventsProfiler->StartProfilerTimer();
	}

	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent)) {
		ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
		if (sdlEvent.type == SDL_EventType::SDL_QUIT) {
			isRunning = false;
			return;
		}
		else if (sdlEvent.type == SDL_KEYDOWN) {
			switch (sdlEvent.key.keysym.scancode) {
				// Toggle Menus
			case SDL_SCANCODE_TAB:
				showSceneMenu = !showSceneMenu;
				currentScene->showMenu = showSceneMenu;
				break;

			case SDL_SCANCODE_ESCAPE:
				isRunning = false;
				return;
				[[fallthrough]]; /// C17 Prevents switch/case fallthrough warnings
			default:
				break;
			}
		}
		if (currentScene == nullptr) {
			Debug::FatalError("Failed to initialize Scene", __FILE__, __LINE__);
			isRunning = false;
			return;
		}
		currentScene->HandleEvents(sdlEvent);
	}

	if (enableProfilers)
	{
		HEPF_Time = handleEventsProfiler->EndProfilerTimer("HandleEvents");
		HEPF_AvgTime = handleEventsProfiler->GetFunctionAvgTime("HandleEvents");
	}
}

void SceneManager::Update()
{
	timer->UpdateFrameTicks();

	if (enableProfilers)
	{
		updateProfiler->StartProfilerTimer();
	}

	currentScene->Update(timer->GetDeltaTime());

	if (enableProfilers)
	{
		UPF_Time = updateProfiler->EndProfilerTimer("Update");
		UPF_AvgTime = updateProfiler->GetFunctionAvgTime("Update");
	}
}

void SceneManager::Render()
{
	if (enableProfilers)
	{
		renderProfiler->StartProfilerTimer();
	}

	currentScene->Render();

	if (enableProfilers)
	{
		RPF_Time = renderProfiler->EndProfilerTimer("Render");
		RPF_AvgTime = renderProfiler->GetFunctionAvgTime("Render");
	}
}

void SceneManager::HandleGUI()
{
	// ImGui New Frame
	switch (rendererType)
	{
	case RendererType::OPENGL:
		ImGui_ImplOpenGL3_NewFrame();
		break;
	case RendererType::VULKAN:
		// TODO:: Implement Vulkan Renderer ImGui
		break;
	default:
		break;
	}

	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	if (show_demo_window) {
		ImGui::ShowDemoWindow(&show_demo_window);
	}

	// Display Scene Menu
	if (showSceneMenu)
	{
		bool open = true;
		ImGui::Begin("Scene", &open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
		ImGui::SetWindowSize(ImVec2(600.0f, 300.0f));
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		if (ImGui::CollapsingHeader("Scene Manager Settings"))
		{
			if (ImGui::TreeNode("Settings"))
			{
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Profilers"))
			{
				ImGui::Checkbox("Enable Profilers", &enableProfilers);

				// Display profiler data
				if (enableProfilers)
				{
					ImGui::Text("Render Profiler Time: %lld Microseconds", RPF_Time);
					ImGui::Text("Render Profiler Average Time: %lld Microseconds", RPF_AvgTime);
					ImGui::Text("Update Profiler Time: %lld Microseconds", UPF_Time);
					ImGui::Text("Update Profiler Average Time: %lld Microseconds", UPF_AvgTime);
					ImGui::Text("Handle Events Profiler Time: %lld Microseconds", HEPF_Time);
					ImGui::Text("Handle Events Profiler Average Time: %lld Microseconds", HEPF_AvgTime);
				}

				ImGui::TreePop();
			}
		}
		currentScene->HandleGUI();

		if (!currentScene || !currentScene->showMenu)
		{
			ImGui::End();
		}
	}

	// Show Framerate
	bool open = true;
	ImGui::Begin("Frame rate", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);
	ImGui::Text("%.1f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();

	// ImGui Render
	ImGui::Render();
	switch (rendererType)
	{
	case RendererType::NONE:
		break;
	case RendererType::OPENGL:
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		break;
	case RendererType::VULKAN:
		break;
	case RendererType::DIRECTX11:
		break;
	case RendererType::DIRECTX12:
		break;
	default:
		break;
	}
}

void SceneManager::BuildNewScene(SCENE_NUMBER scene) {
	bool status; 

	if (currentScene != nullptr) {
		currentScene->OnDestroy();
		delete currentScene;
		currentScene = nullptr;
	}

	switch (scene) {
	case SCENE_NUMBER::SCENE0:
		currentScene = new Scene0(renderer);
		currentScene->controllerManager = controllerManager;
		status = currentScene->OnCreate();
		break;

	default:
		Debug::Error("Incorrect scene number assigned in the manager", __FILE__, __LINE__);
		currentScene = nullptr;
		break;
	}	
}

void SceneManager::showProfilerMenu()
{

}


