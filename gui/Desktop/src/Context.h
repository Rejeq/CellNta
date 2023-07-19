#pragma once

#include <cfloat>

#include <iomanip>
#include <string>
#include <vector>

#include <imgui.h>
#include <imgui_internal.h>

#include <Cellnta/Renderer/Renderer.h>

#include "ActionBase.h"
#include "Keybind.h"
#include "UndoRedo.h"
#include "WindowBase.h"

namespace Cellnta {

class World;
enum class WorldType;

}  // namespace Cellnta

namespace Ui {

class Context {
  using WindowsDataType = std::vector<std::unique_ptr<Window>>;

 public:
  Context();

  void AddWindow(std::unique_ptr<Window>&& window);
  void SetDimension(int dim);

  template <typename ActionType, typename... Args>
  Action::BasePtr MakeAction(Args&&... args) {
    auto action = Action::Make<ActionType>(std::forward<Args>(args)...);
    action->SetContext(this);
    return action;
  }

  template <int Hint = ActionHint::None>
  void PushAction(Action::BasePtr&& action) {
    action->SetContext(this);
    action->Execute();

    if (action->CanUndo() && !(Hint & ActionHint::NoUndo)) {
      m_undoRedo.PushAction(std::move(action));
    }
  }

  template <typename ActionType, int Hint = ActionHint::None, typename... Args>
  void PushAction(Args&&... args) {
    constexpr bool IsUndoable = std::is_base_of_v<Action::UndoBase, ActionType>;

    auto ExecuteAction = [&](Action::Base& action) {
      action.SetContext(this);
      action.Execute();
    };

    if constexpr (!IsUndoable || (Hint & ActionHint::NoUndo)) {
      ActionType tmpAction(std::forward<Args>(args)...);
      ExecuteAction(tmpAction);
    } else {
      Action::BasePtr action =
          Action::Make<ActionType>(std::forward<Args>(args)...);
      ExecuteAction(*action);
      m_undoRedo.PushAction(std::move(action));
    }
  }

  void Update();
  void Draw();

  void SetupDockspace();
  ImGuiID GetDockspace() const { return m_dockspaceId; }

  const WindowsDataType& GetWindows() const { return m_windowsData; }
  Window* GetWindowByName(const std::string& name);

  bool SetWorld(const Cellnta::WorldType type);
  const Cellnta::World& GetWorld() const { return *m_world; }
  Cellnta::World& GetWorld() { return *m_world; }

  const Cellnta::Renderer& GetRenderer() const { return m_renderer; }
  Cellnta::Renderer& GetRenderer() { return m_renderer; }

  const UndoRedo& GetUndoRedo() const { return m_undoRedo; }
  UndoRedo& GetUndoRedo() { return m_undoRedo; }

  const KeybindMap& GetKeybindings() const { return m_keybinds; }
  KeybindMap& GetKeybindings() { return m_keybinds; }

  void SetMouseDelta(float xDelta, float yDelta) {
    m_mouseDeltaX = xDelta;
    m_mouseDeltaY = yDelta;
  }

  float GetMouseDeltaX() const { return m_mouseDeltaX; }
  float GetMouseDeltaY() const { return m_mouseDeltaY; }

  void SetOnFirstStartup(
      const std::function<void(const Context&)>& onFirstStartup) {
    m_OnFirstStartup = onFirstStartup;
  }

 private:
  void ReadWindowProperties(ImGuiSettingsHandler* handler,
                            ImGuiTextBuffer* buf);
  void WriteWindowProperties(ImGuiSettingsHandler* handler,
                             ImGuiTextBuffer* buf);

  static void SettingsHandler_WriteAll(ImGuiContext*,
                                       ImGuiSettingsHandler* handler,
                                       ImGuiTextBuffer* buf);
  static void SettingsHandler_ReadLine(ImGuiContext*,
                                       ImGuiSettingsHandler* handler,
                                       void* entry, const char* line);
  static void* SettingsHandler_ReadOpen(ImGuiContext*, ImGuiSettingsHandler*,
                                        const char* name);

  void ProccessKeybindings();

  WindowsDataType m_windowsData;
  Cellnta::Renderer m_renderer;
  std::unique_ptr<Cellnta::World> m_world;
  std::shared_ptr<Cellnta::Camera3d> m_camera3d;
  std::shared_ptr<Cellnta::CameraNdList> m_cameraNd;
  std::shared_ptr<Cellnta::HypercubeStorage> m_hypercube;
  std::shared_ptr<Cellnta::RenderData> m_renderData;

  UndoRedo m_undoRedo;
  KeybindMap m_keybinds;

  std::string m_currentWindow;

  std::function<void(const Context&)> m_OnFirstStartup;

  ImGuiID m_dockspaceId = 0;

  float m_mouseDeltaX = 0;
  float m_mouseDeltaY = 0;
  bool m_firstStartup = true;
};

}  // namespace Ui
