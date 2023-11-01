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
	fps(60), isRunning(false), fullScreen(false), show_demo_window(true),
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

	updateProfiler = std::make_shared<Profiler>(enableProfilers);
	if (updateProfiler == nullptr)
	{
		Debug::FatalError("Failed to initialize Update Profiler", __FILE__, __LINE__);
		return false;
	}
	handleEventsProfiler = std::make_shared<Profiler>(enableProfilers);
	if (handleEventsProfiler == nullptr)
	{
		Debug::FatalError("Failed to initialize Handle Events Profiler", __FILE__, __LINE__);
		return false;
	}
	renderProfiler = std::make_shared<Profiler>(enableProfilers);
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

	handleEventsProfiler->StartProfilerTimer();

	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent)) {
		ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
		if (sdlEvent.type == SDL_EventType::SDL_QUIT) {
			isRunning = false;
			return;
		}
		else if (sdlEvent.type == SDL_KEYDOWN) {
			switch (sdlEvent.key.keysym.scancode) {
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
	handleEventsProfiler->EndProfilerTimer("HandleEvents");
	handleEventsProfiler->PrintFunctionAvgTime("HandleEvents");
}

void SceneManager::Update()
{
	timer->UpdateFrameTicks();

	updateProfiler->StartProfilerTimer();

	currentScene->Update(timer->GetDeltaTime());

	updateProfiler->EndProfilerTimer("Update");
	updateProfiler->PrintFunctionAvgTime("Update");
}

void SceneManager::Render()
{
	renderProfiler->StartProfilerTimer();

	currentScene->Render();

	renderProfiler->EndProfilerTimer("Render");
	renderProfiler->PrintFunctionAvgTime("Render");
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

	currentScene->HandleGUI();

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


