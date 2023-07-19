#include "Keybind.h"

#include "Context.h"
#include "Menubar/AppearanceAction.h"

using namespace Ui;

void KeybindMap::Add(const Keymap& map, Func func) {
  auto res = m_data.insert_or_assign(map, func);
  if (res.second == false)
    DESKTOP_LOG_WARN("Keybind ({}, {}) has been redefined", (int)map.key,
                     (int)map.mod);
}

KeybindMap::Func KeybindMap::Erase(const Keymap& map) {
  auto res = m_data.extract(map);

  if (res.empty())
    return nullptr;
  return std::move(res.mapped());
}

KeybindMap::Func KeybindMap::Find(const Keymap& map) {
  auto res = m_data.find(map);

  if (res == m_data.end())
    return nullptr;
  return res->second;
}

void KeybindMap::SetupDefault() {
  auto AddSeveral = [&]<typename... Args>(Func func, Args... keys) {
    for (auto key : {keys...})
      Add(key, func);
  };

  Add(ImGuiKey_F11, [](Context& ctx) {
    ctx.PushAction<Action::Appearance::ToggleFullscreen>();
  });

  Add(Keymap(ImGuiKey_Z, ImGuiMod_Ctrl), [](Context& ctx) {
    auto& undoRedo = ctx.GetUndoRedo();
    if (undoRedo.CanUndo())
      undoRedo.Undo();
  });

  AddSeveral(
      [](Context& ctx) {
        auto& undoRedo = ctx.GetUndoRedo();
        if (undoRedo.CanRedo())
          undoRedo.Redo();
      },
      Keymap(ImGuiKey_Y, ImGuiMod_Ctrl),
      Keymap(ImGuiKey_Z, ImGuiMod_Ctrl | ImGuiMod_Shift));
}
