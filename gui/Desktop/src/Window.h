#pragma once

#include <imgui.h>

namespace Ui
{
  class Context;

  struct WindowProperties
  {
    const char* Name = nullptr;
    bool Opened = true;
    bool VisibleInMenubar = true;
  };

  class Window
  {
  public:
    Window(const WindowProperties& properties)
      : p_prop(properties) {}
    virtual ~Window() {}

    virtual void Draw() = 0;
    virtual void ReadProperties(const char* line)
    {
      int opened;
      if (sscanf(line, "Opened=%i", &opened) == 1)
        p_prop.Opened = opened;
    }

    virtual void WriteProperties(ImGuiTextBuffer* buf) const
    {
      buf->appendf("Opened=%i\n", p_prop.Opened);
    }

    void SetContext(Context* ctx)
    {
      m_ctx = ctx;
    }

    const WindowProperties& GetProperties() const { return p_prop; }
    WindowProperties& GetProperties() { return p_prop; }

    Context* GetContext() { return m_ctx; }
    const Context* GetContext() const { return m_ctx; }

  protected:
    WindowProperties p_prop;

  private:
    Context* m_ctx = nullptr;
  };
}
