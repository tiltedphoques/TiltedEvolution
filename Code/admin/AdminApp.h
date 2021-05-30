#pragma once

#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/Sdl2Application.h>

using namespace Magnum;
using namespace Math::Literals;


class AdminApp : public Platform::Application
{
  public:
    explicit AdminApp(const Arguments& arguments);

    void drawEvent() override;
    void tickEvent() override;

    void viewportEvent(ViewportEvent& event) override;

    void keyPressEvent(KeyEvent& event) override;
    void keyReleaseEvent(KeyEvent& event) override;

    void mousePressEvent(MouseEvent& event) override;
    void mouseReleaseEvent(MouseEvent& event) override;
    void mouseMoveEvent(MouseMoveEvent& event) override;
    void mouseScrollEvent(MouseScrollEvent& event) override;
    void textInputEvent(TextInputEvent& event) override;

  private:
    ImGuiIntegration::Context m_imgui{NoCreate};

    bool m_showAnotherWindow = false;
    Color4 m_clearColor = 0x72909aff_rgbaf;
    Float m_floatValue = 0.0f;
};
