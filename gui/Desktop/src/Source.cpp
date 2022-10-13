#include <SDL.h>

#include <imgui.h>
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <Config.h>
#include <GlBackend.h>
#include <Canvas.h>

#include "Context.h"
#include "RendererWindow.h"
#include "AlgoWindow.h"

#ifdef NO_SDL_MAIN
#undef main
#endif

constexpr size_t WINDOW_WIDTH = 1200;
constexpr size_t WINDOW_HEIGHT = 700;

const char* GlDebugSourceToString(GLenum source)
{
  switch (source)
  {
  case GL_DEBUG_SOURCE_API:             return "API";
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   return "WINDOW_SYSTEM";
  case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER_COMPILER";
  case GL_DEBUG_SOURCE_THIRD_PARTY:     return "THIRD_PARTY";
  case GL_DEBUG_SOURCE_APPLICATION:     return "APPLICATION";
  case GL_DEBUG_SOURCE_OTHER:           return "OTHER";
  }
  return "Undefined";
}

const char* GlDebugTypeToString(GLenum type)
{
  switch (type)
  {
  case GL_DEBUG_TYPE_ERROR:               return "GL_ERROR";
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "UNDEFINED_BEHAVIOR";
  case GL_DEBUG_TYPE_PORTABILITY:         return "PORTABILITY";
  case GL_DEBUG_TYPE_PERFORMANCE:         return "PERFORMANCE";
  case GL_DEBUG_TYPE_MARKER:              return "MARKER";
  case GL_DEBUG_TYPE_PUSH_GROUP:          return "PUSH_GROUP";
  case GL_DEBUG_TYPE_POP_GROUP:           return "POP_GROUP";
  case GL_DEBUG_TYPE_OTHER:               return "OTHER";
  }
  return "Undefined";
}

void GlMessageCallback(
	GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length,
	const GLchar* message, const void* userParam)
{
  //Since the shader error handled in the Shader class
  if(source == GL_DEBUG_SOURCE_SHADER_COMPILER)
    return;

  const char* msg = "GL_Message: {}: {}: [{}]:\n{}\n";
#define MESSAGE_ARGS GlDebugSourceToString(source), GlDebugTypeToString(type), id, message
  switch (severity)
  {
  case GL_DEBUG_SEVERITY_HIGH:         LOG_ERROR(msg, MESSAGE_ARGS); break;
  case GL_DEBUG_SEVERITY_MEDIUM:       LOG_WARN(msg, MESSAGE_ARGS); break;
  case GL_DEBUG_SEVERITY_LOW:          LOG_INFO(msg, MESSAGE_ARGS); break;
  case GL_DEBUG_SEVERITY_NOTIFICATION: LOG_TRACE(msg, MESSAGE_ARGS); break;
  }
#undef MESSAGE_ARGS
}

int InitSdlAndCreateGlWindow(SDL_Window*& win, SDL_GLContext& glCtx)
{
  ProfileScope;

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    LOG_CRITICAL("Unable to initialize SDL2: {}", SDL_GetError());
    return EXIT_FAILURE;
  }
  //SDL_SetHintWithPriority(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1", SDL_HINT_OVERRIDE);

#ifdef __ANDROID__
  SDL_SetHintWithPriority(SDL_HINT_ORIENTATIONS, "LandscapeRight", SDL_HINT_OVERRIDE);
#endif

#if defined(CELLNTA_RENDERER_GLES3)
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#elif defined(CELLNTA_RENDERER_GL)
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
#error "Renderer not provided"
#endif
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  uint32_t winFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
#ifdef CELLNTA_RENDERER_GLES3
  winFlags |= SDL_WINDOW_FULLSCREEN;
#endif

  win = SDL_CreateWindow("CellNta", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, winFlags);
  if (win == nullptr) {
    LOG_CRITICAL("Unable to create window: {}", SDL_GetError());
    return EXIT_FAILURE;
  }

  glCtx = SDL_GL_CreateContext(win);

#ifdef CELLNTA_RENDERER_GL
  GLint glewError = glewInit();
  if (glewError != GLEW_OK)
  {
    LOG_CRITICAL("Unable to initialize GLEW: {}",
        (const char*) glewGetErrorString(glewError));
    return EXIT_FAILURE;
  }
#endif
  SDL_GL_MakeCurrent(win, glCtx);
  return 0;
}

int InitImGui(SDL_Window*& win, SDL_GLContext& glCtx)
{
  ProfileScope;

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  ImGui::StyleColorsDark();
  //ImGui::StyleColorsLight();

  ImGui_ImplSDL2_InitForOpenGL(win, glCtx);
#if defined(CELLNTA_RENDERER_GLES3)
  ImGui_ImplOpenGL3_Init("#version 300 es");
#elif defined(CELLNTA_RENDERER_GL)
  ImGui_ImplOpenGL3_Init("#version 430 core");
#else
#error "Renderer not provided"
#endif
  return 0;
}

void CanvasProcessMouseEvents(SDL_Event& event, Canvas& canvas)
{
  ImGuiIO& io = ImGui::GetIO();
  float& delta = io.DeltaTime;

  //TODO:
  //if (Ui::GetContext()->SceneWindowFocused)
  {
    if (SDL_GetRelativeMouseMode())
    {
      if (event.type == SDL_MOUSEMOTION)
        canvas.OnMouseMotion(event.motion.xrel, event.motion.yrel, delta);

      if (event.type == SDL_MOUSEWHEEL)
        canvas.OnMove(MoveDirection::FORWARD, event.wheel.y / 4.0f);
    }
  }
}

void CanvasProcessKeyEvents(Canvas& canvas)
{
  ImGuiIO& io = ImGui::GetIO();
  float& delta = io.DeltaTime;

  //TODO:
  //if (Ui::GetContext()->SceneWindowFocused)
  {
    const uint8_t* state = SDL_GetKeyboardState(nullptr);
    if (state != nullptr)
    {
      if (state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_W])
        canvas.OnMove(MoveDirection::FORWARD, delta);
      if (state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_A])
        canvas.OnMove(MoveDirection::LEFT, delta);
      if (state[SDL_SCANCODE_DOWN] || state[SDL_SCANCODE_S])
        canvas.OnMove(MoveDirection::BACKWARD, delta);
      if (state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_D])
        canvas.OnMove(MoveDirection::RIGHT, delta);

      if (state[SDL_SCANCODE_SPACE])
        canvas.OnMove(MoveDirection::WORLD_UP, delta);
      if (state[SDL_SCANCODE_LSHIFT])
        canvas.OnMove(MoveDirection::WORLD_DOWN, delta);
    }
  }
}

void CreateSceneFramebuffer(Ui::Context& ctx, uint32_t& fb, uint32_t& texColor, uint32_t& texDepth)
{
  if (fb != 0)
    glDeleteFramebuffers(1, &fb);
  if (texColor != 0)
    glDeleteTextures(1, &texColor);
  if (texDepth != 0)
    glDeleteTextures(1, &texDepth);

  glGenFramebuffers(1, &fb);
  glBindFramebuffer(GL_FRAMEBUFFER, fb);

  auto size = ctx.GetCanvas().GetCamera3d().GetSizeBase();
  LOG_INFO("Scene framebuffer resized: ({}; {})", size.x(), size.y());

  glGenTextures(1, &texColor);
  glBindTexture(GL_TEXTURE_2D, texColor);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x(), size.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColor, 0);

  glGenTextures(1, &texDepth);
  glBindTexture(GL_TEXTURE_2D, texDepth);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, size.x(), size.y());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texDepth, 0);

  ctx.SceneTextureId = texColor;
}


int main(int, char**)
{
  SDL_Window* win = nullptr;
  SDL_GLContext glCtx;

  SDL_Delay(1000);

  Log::InitDefault();
  Log::GetLogger()->set_level(spdlog::level::debug);

  if (int ret = InitSdlAndCreateGlWindow(win, glCtx) != 0)
    return ret;
  if (int ret = InitImGui(win, glCtx) != 0)
    return ret;

  SDL_GL_SetSwapInterval(1); // Enable vsync
//#if !LF_TARGET_GLES3
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(GlMessageCallback, 0);
//#endif
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

  int winWidth = 0;
  int winHeight = 0;
  SDL_GetWindowSize(win, &winWidth, &winHeight);

	std::unique_ptr<Canvas> canvasPtr = std::make_unique<Canvas>(Lf::AlgoType::RANDOM, 3);
	canvasPtr->GetCamera3d().SetPosition(Eigen::Vector3f(0.0f, 2.0f, 0.0f));
	canvasPtr->GetCamera3d().Resize(Eigen::Vector2f(winWidth, winHeight));

  Ui::Context ctx(std::move(canvasPtr));
  ctx.AddWindow(std::make_unique<Ui::RendererWindow>());
  ctx.AddWindow(std::make_unique<Ui::AlgoWindow>());

  Canvas& canvas = ctx.GetCanvas();

  uint32_t sceneFb = 0;
  uint32_t sceneTextColor = 0;
  uint32_t sceneTextDepth = 0;
  CreateSceneFramebuffer(ctx, sceneFb, sceneTextColor, sceneTextDepth);

	SDL_Event event;
	bool done = false;
	bool windowFocused = false;
  bool windowMinimized = false;

  ImGuiIO& io = ImGui::GetIO();

  size_t timeFrequency = SDL_GetPerformanceFrequency();
  size_t frameStart = 0;
  size_t frameEnd = 0;
  float frameDelta = 0;

	while (!done)
	{
    frameStart = SDL_GetPerformanceCounter();
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);

      if (windowFocused && (SDL_GetRelativeMouseMode() == SDL_TRUE))
        CanvasProcessMouseEvents(event, canvas);

			switch (event.type)
			{
			case SDL_QUIT:
				done = true;
				break;
			case SDL_MOUSEBUTTONDOWN:
        //TODO
				if (windowFocused && io.KeyCtrl)
				{
          if (SDL_GetRelativeMouseMode())
          {
              SDL_SetRelativeMouseMode(SDL_FALSE);
              io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
          }
          else
          {
            if (ctx.SceneWindowFocused)
            {
              SDL_SetRelativeMouseMode(SDL_TRUE);
              io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
            }
          }
				}
				break;
			case SDL_WINDOWEVENT:
				if (event.window.windowID != SDL_GetWindowID(win))
					break;
				switch (event.window.event)
				{
				case SDL_WINDOWEVENT_ENTER:
					windowFocused = true;
					break;
				case SDL_WINDOWEVENT_LEAVE:
					windowFocused = false;
					break;
				case SDL_WINDOWEVENT_RESIZED:
					canvas.OnResize(event.window.data1, event.window.data2);
          CreateSceneFramebuffer(ctx, sceneFb, sceneTextColor, sceneTextDepth);
					break;
        case SDL_WINDOWEVENT_MINIMIZED:
          windowMinimized = true;
          break;
        case SDL_WINDOWEVENT_RESTORED:
          windowMinimized = false;
          break;
				case SDL_WINDOWEVENT_CLOSE:
					done = true;
					break;
				default:
					break;
				}
				break;
			default: break;
			}
		}

    if (ImGui::IsKeyPressed(ImGuiKey_Escape))
    {
      done = true;
      continue;
    }

    if (windowFocused && (SDL_GetRelativeMouseMode() == SDL_TRUE))
      CanvasProcessKeyEvents(canvas);

    if (ImGui::IsKeyPressed(ImGuiKey_Q, true))
      canvas.Rotate(0, 0, 0);

		canvas.Update(frameDelta);

    if (!windowMinimized)
    {
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplSDL2_NewFrame();
      ImGui::NewFrame();
      ctx.MainDockspaceId = ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

      ImGui::ShowDemoWindow();
      ctx.Draw();

      ImGui::Render();

      glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
      glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
#if LF_TARGET_GLES3
      glClearDepthf(1.0f);
#else
      glClearDepth(1.0f);
#endif
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      if (canvas.WantDraw())
      {
        glBindFramebuffer(GL_FRAMEBUFFER, sceneFb);
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        canvas.RenderWorld();
        canvas.RenderGrid();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
      }
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    SDL_GL_SwapWindow(win);

    frameEnd = SDL_GetPerformanceCounter();
    frameDelta = ((float) frameEnd - frameStart) / timeFrequency;

    ProfileFrame;
	}

  ProfileScopeN("Shutdown");

  ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(glCtx);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
