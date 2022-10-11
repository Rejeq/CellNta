#pragma once

#include <imgui.h>

namespace Ui
{
  class Context;

  class Window
  {
  public:
    Window(const char* name)
      : p_name(name) {}
    virtual ~Window() {}

    virtual void Draw() = 0;
    virtual void ReadProperties(const char* line)
    {
      int opened;
      if (sscanf(line, "Opened=%i", &opened) == 1)
        p_open = opened;
    }

    virtual void WriteProperties(ImGuiTextBuffer* buf) const
    {
      buf->appendf("Opened=%i\n", p_open);
    }

    void SetContext(Context* ctx)
    {
      m_ctx = ctx;
    }

    void SetOpened(bool opened) { p_open = opened; }
    bool& GetOpened() { return p_open; }
    bool GetOpened() const { return p_open; }

    const char* GetName() const { return p_name; }
    Context* GetContext() { return m_ctx; }
    const Context* GetContext() const { return m_ctx; }

  protected:
    const char* p_name = nullptr;
    bool p_open = true;

  private:
    Context* m_ctx = nullptr;
  };
}
