#include <SDL.h>

#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include <imgui.h>

#include <Cellnta/Config.h>
#include <Cellnta/Renderer/Camera3d.h>
#include <Cellnta/Renderer/GlBackend.h>
#include <Cellnta/Renderer/HypercubeStorage.h>

#include "Context.h"
#include "Log.h"
#include "View/Camera/Window.h"
#include "View/Menubar/Window.h"
#include "View/Pattern/Window.h"
#include "View/Renderer/Window.h"
#include "View/Scene/Window.h"
#include "View/World/Window.h"

#ifndef RESOURCE_DIR
#error "Not provided resource directory"
#define RESOURCE_DIR
#endif

#ifdef main
#undef main
#endif

constexpr size_t WINDOW_WIDTH = 1200;
constexpr size_t WINDOW_HEIGHT = 700;

const char* GlDebugSourceToString(GLenum source) {
  switch (source) {
    case GL_DEBUG_SOURCE_API: return "API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW_SYSTEM";
    case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER_COMPILER";
    case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD_PARTY";
    case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
    case GL_DEBUG_SOURCE_OTHER: return "OTHER";
  }
  return "Undefined";
}

const char* GlDebugTypeToString(GLenum type) {
  switch (type) {
    case GL_DEBUG_TYPE_ERROR: return "GL_ERROR";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
    case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
    case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
    case GL_DEBUG_TYPE_MARKER: return "MARKER";
    case GL_DEBUG_TYPE_PUSH_GROUP: return "PUSH_GROUP";
    case GL_DEBUG_TYPE_POP_GROUP: return "POP_GROUP";
    case GL_DEBUG_TYPE_OTHER: return "OTHER";
  }
  return "Undefined";
}

void GlMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                       GLsizei /*length*/, const GLchar* message,
                       const void* /*userParam*/) {
  // Since the shader error handled in the Shader class
  if (source == GL_DEBUG_SOURCE_SHADER_COMPILER)
    return;

  constexpr std::string_view msg = "{}: {}: [{}]:\n{}\n";
#define MESSAGE_ARGS \
  GlDebugSourceToString(source), GlDebugTypeToString(type), id, message

  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH: DESKTOP_GL_LOG_ERROR(msg, MESSAGE_ARGS); break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      DESKTOP_GL_LOG_WARN(msg, MESSAGE_ARGS);
      break;
    case GL_DEBUG_SEVERITY_LOW: DESKTOP_GL_LOG_INFO(msg, MESSAGE_ARGS); break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      DESKTOP_GL_LOG_TRACE(msg, MESSAGE_ARGS);
      break;
  }
#undef MESSAGE_ARGS
}

void Shutdown(SDL_Window* win, SDL_GLContext glCtx) {
  CELLNTA_PROFILE;

  DESKTOP_LOG_INFO("Shutting down the application");

  if (ImGui::GetCurrentContext()) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
  }

  SDL_GL_DeleteContext(glCtx);
  SDL_DestroyWindow(win);
  SDL_Quit();
}

bool InitLogging() {
  if (Log::InitDefault())
    return true;
  Log::GetLogger()->set_level(spdlog::level::trace);

  if (LogGL::InitDefault())
    DESKTOP_LOG_ERROR("Unable to initilize OpenGL logger");
  LogGL::GetLogger()->set_level(spdlog::level::trace);

  if (Cellnta::Log::InitDefault())
    DESKTOP_LOG_ERROR("Unable to initilize Cellnta logger");
  Cellnta::Log::GetLogger()->set_level(spdlog::level::trace);

  return false;
}

bool CreateGlWindow(SDL_Window*& win, SDL_GLContext& glCtx) {
  CELLNTA_PROFILE;

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
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

  uint32_t winFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
#ifdef CELLNTA_RENDERER_GLES3
  winFlags |= SDL_WINDOW_FULLSCREEN;
#endif

  win = SDL_CreateWindow("CellNta", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT,
                         winFlags);
  if (win == nullptr) {
    DESKTOP_LOG_CRITICAL("Unable to create window: {}", SDL_GetError());
    return true;
  }

  glCtx = SDL_GL_CreateContext(win);
  if (glCtx == nullptr) {
    DESKTOP_LOG_CRITICAL("Unable to create gl context: {}", SDL_GetError());
    return true;
  }

#ifdef CELLNTA_RENDERER_GL
  GLint glewError = glewInit();
  if (glewError != GLEW_OK) {
    DESKTOP_LOG_CRITICAL("Unable to initialize GLEW: {}",
                         (const char*)glewGetErrorString(glewError));
    return true;
  }
#endif
  SDL_GL_MakeCurrent(win, glCtx);

  return false;
}

bool InitImGui(SDL_Window*& win, SDL_GLContext& glCtx) {
  CELLNTA_PROFILE;

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  ImGui_ImplSDL2_InitForOpenGL(win, glCtx);
#if defined(CELLNTA_RENDERER_GLES3)
  ImGui_ImplOpenGL3_Init("#version 300 es");
#elif defined(CELLNTA_RENDERER_GL)
  ImGui_ImplOpenGL3_Init("#version 430 core");
#else
#error "Renderer not provided"
#endif
  return false;
}

void ResetContextLayout(const Ui::Context& ctx) {
  ImGuiID dockId = ctx.GetDockspace();

  ImGui::DockBuilderRemoveNode(dockId);
  ImGui::DockBuilderAddNode(dockId, ImGuiDockNodeFlags_DockSpace);
  ImGui::DockBuilderGetNode(dockId)->LocalFlags &=
      ~ImGuiDockNodeFlags_CentralNode;

  constexpr float size = 0.25f;
  ImGuiID dockIdLeft = ImGui::DockBuilderSplitNode(dockId, ImGuiDir_Left, size,
                                                   nullptr, &dockId);
  ImGuiID dockIdRem = ImGui::DockBuilderSplitNode(
      dockId, ImGuiDir_Right, 1.0f - size, nullptr, &dockId);

  // TODO: Make independent of window name
  ImGui::DockBuilderDockWindow("Scene", dockIdRem);
  // TODO: Delete this
  ImGui::DockBuilderDockWindow("Dear ImGui Demo", dockIdRem);

  ImGui::DockBuilderDockWindow("Renderer", dockIdLeft);
  ImGui::DockBuilderDockWindow("World", dockIdLeft);

  ImGui::DockBuilderFinish(dockId);
}

bool CreateContextLayout(Ui::Context& ctx) {
  Cellnta::Renderer& ren = ctx.GetRenderer();
  Cellnta::Camera3d* cam3d = ren.GetCamera3d();
  Cellnta::HypercubeStorage* cube = ren.GetHypercube();

  ctx.SetWorld(Cellnta::WorldType::SIMPLE);
  ctx.SetDimension(2);

  bool err = ren.CreateShaders(RESOURCE_DIR "Shaders/Grid.glsl",
                               RESOURCE_DIR "Shaders/Cell.glsl");
  if (err) {
    DESKTOP_LOG_CRITICAL("Unable to load shaders");
    return true;
  }

  if (cam3d == nullptr)
    DESKTOP_LOG_ERROR("Camera not initialized");
  else
    cam3d->SetPosition(Eigen::Vector3f(0.0f, 2.0f, 0.0f));

  ren.SetRenderDistance(256);
  if (cube == nullptr)
    DESKTOP_LOG_ERROR("Hypercube not initialized");
  else
    cube->GenerateCube(ren.GetDimensions(), 0.5f, Cellnta::CubeMode::WIREFRAME);

  ctx.SetOnFirstStartup(ResetContextLayout);

  ctx.AddWindow(std::make_unique<Ui::CameraWindow>());
  ctx.AddWindow(std::make_unique<Ui::RendererWindow>());
  ctx.AddWindow(std::make_unique<Ui::WorldWindow>());
  ctx.AddWindow(std::make_unique<Ui::SceneWindow>());
  ctx.AddWindow(std::make_unique<Ui::MenubarWindow>(ResetContextLayout));
  ctx.AddWindow(std::make_unique<Ui::PatternWindow>());

  return false;
}

int main(int /*unused*/, char** /*unused*/) {
  SDL_Window* win = nullptr;
  SDL_GLContext glCtx = nullptr;

  if (InitLogging()) {
    printf("Unable to initialize logging");
    return EXIT_FAILURE;
  }

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    DESKTOP_LOG_CRITICAL("Unable to initialize SDL2: {}", SDL_GetError());
    return EXIT_FAILURE;
  }

  if (CreateGlWindow(win, glCtx)) {
    Shutdown(win, glCtx);
    return EXIT_FAILURE;
  }

  if (InitImGui(win, glCtx)) {
    Shutdown(win, glCtx);
    return EXIT_FAILURE;
  }

  SDL_GL_SetSwapInterval(1);  // Enable vsync

  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(GlMessageCallback, nullptr);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);

#if defined(CELLNTA_RENDERER_GL)
  glPointSize(10.0f);
#else
#error "Add support for glPointSize"
#endif

  Ui::Context ctx;
  if (CreateContextLayout(ctx)) {
    Shutdown(win, glCtx);
    return EXIT_FAILURE;
  }

  SDL_Event event;
  bool done = false;
  bool windowMinimized = false;

  ImGuiIO& io = ImGui::GetIO();

  while (!done) {
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);

      switch (event.type) {
        case SDL_QUIT: done = true; break;
        case SDL_WINDOWEVENT:
          switch (event.window.event) {
            case SDL_WINDOWEVENT_MINIMIZED: windowMinimized = true; break;
            case SDL_WINDOWEVENT_RESTORED: windowMinimized = false; break;
            default: break;
          }
          break;
        default: break;
      }
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
      done = true;
      continue;
    }

    ctx.Update();

    if (!windowMinimized) {
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplSDL2_NewFrame();
      ImGui::NewFrame();

      // Add additional windows after ctx.Draw()
      // because it creates dock space
      ctx.Draw();

      ImGui::Render();

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    SDL_GL_SwapWindow(win);

    CELLNTA_PROFILE_FRAME;
  }

  Shutdown(win, glCtx);
  return EXIT_SUCCESS;
}
