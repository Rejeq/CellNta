#pragma once

#include <unordered_map>

#include <imgui.h>

#include "ActionBase.h"

namespace Ui {

class Context;

struct Keymap {
  ImGuiKey key = ImGuiKey_None;
  int mod = ImGuiMod_None;

  Keymap() = default;
  Keymap(ImGuiKey key) : key(key) {}
  Keymap(ImGuiKey key, ImGuiKey mod) : key(key), mod(mod) {}
  Keymap(ImGuiKey key, int mod) : key(key), mod(mod) {}

  bool operator==(const Keymap& r) const {
    return (key == r.key && mod == r.mod);
  }

  bool operator!=(const Keymap& r) const {
    return !(*this == r);
  }
};

struct KeymapHasher
{
  std::size_t operator()(const Keymap& map) const
  {
    int hash = 17;
    hash = hash * 31 + std::hash<int>()(map.key);
    hash = hash * 31 + std::hash<int>()(map.mod);
    return hash;
  }
};

class KeybindMap {
 public:
  using Func = std::function<void(Context&)>;

  void Add(const Keymap& map, Func func);

  Func Erase(const Keymap& key);
  Func Find(const Keymap& key);

  void SetupDefault();

 private:
  std::unordered_map<Keymap, Func, KeymapHasher> m_data;
};

}  // namespace Ui
