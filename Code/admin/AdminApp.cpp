#include "AdminApp.h"

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>

AdminApp::AdminApp(const Arguments& arguments)
    : Platform::Application{
          arguments,
          Configuration{}.setTitle("TiltedOnline Server Admin").setWindowFlags(Configuration::WindowFlag::Resizable)}
{
    m_imgui = ImGuiIntegration::Context(Vector2{windowSize()} / dpiScaling(), windowSize(), framebufferSize());

    /* Set up proper blending to be used by ImGui. There's a great chance
       you'll need this exact behavior for the rest of your scene. If not, set
       this only for the drawFrame() call. */
    GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add, GL::Renderer::BlendEquation::Add);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
                                   GL::Renderer::BlendFunction::OneMinusSourceAlpha);

#if !defined(MAGNUM_TARGET_WEBGL) && !defined(CORRADE_TARGET_ANDROID)
    /* Have some sane speed, please */
    setMinimalLoopPeriod(16);
#endif
}

void AdminApp::drawEvent()
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

    m_imgui.newFrame();

    /* Enable text input, if needed */
    if (ImGui::GetIO().WantTextInput && !isTextInputActive())
        startTextInput();
    else if (!ImGui::GetIO().WantTextInput && isTextInputActive())
        stopTextInput();

    /* 1. Show a simple window.
       Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appear in
       a window called "Debug" automatically */
    {
        ImGui::Text("Hello, world!");
        ImGui::SliderFloat("Float", &m_floatValue, 0.0f, 1.0f);
        if (ImGui::ColorEdit3("Clear Color", m_clearColor.data()))
            GL::Renderer::setClearColor(m_clearColor);
        if (ImGui::Button("Another Window"))
            m_showAnotherWindow ^= true;
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0 / Double(ImGui::GetIO().Framerate),
                    Double(ImGui::GetIO().Framerate));
    }

    /* 2. Show another simple window, now using an explicit Begin/End pair */
    if (m_showAnotherWindow)
    {
        ImGui::SetNextWindowSize(ImVec2(500, 100), ImGuiCond_FirstUseEver);
        ImGui::Begin("Another Window", &m_showAnotherWindow);
        ImGui::Text("Hello");
        ImGui::End();
    }

    /* Update application cursor */
    m_imgui.updateApplicationCursor(*this);

    /* Set appropriate states. If you only draw ImGui, it is sufficient to
       just enable blending and scissor test in the constructor. */
    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);
    GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::disable(GL::Renderer::Feature::DepthTest);

    m_imgui.drawFrame();

    /* Reset state. Only needed if you want to draw something else with
       different state after. */
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::disable(GL::Renderer::Feature::ScissorTest);
    GL::Renderer::disable(GL::Renderer::Feature::Blending);

    swapBuffers();
    redraw();
}

void AdminApp::tickEvent()
{
    
}


void AdminApp::viewportEvent(ViewportEvent& event)
{
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});

    m_imgui.relayout(Vector2{event.windowSize()} / event.dpiScaling(), event.windowSize(), event.framebufferSize());
}

void AdminApp::keyPressEvent(KeyEvent& event)
{
    if (m_imgui.handleKeyPressEvent(event))
        return;
}

void AdminApp::keyReleaseEvent(KeyEvent& event)
{
    if (m_imgui.handleKeyReleaseEvent(event))
        return;
}

void AdminApp::mousePressEvent(MouseEvent& event)
{
    if (m_imgui.handleMousePressEvent(event))
        return;
}

void AdminApp::mouseReleaseEvent(MouseEvent& event)
{
    if (m_imgui.handleMouseReleaseEvent(event))
        return;
}

void AdminApp::mouseMoveEvent(MouseMoveEvent& event)
{
    if (m_imgui.handleMouseMoveEvent(event))
        return;
}

void AdminApp::mouseScrollEvent(MouseScrollEvent& event)
{
    if (m_imgui.handleMouseScrollEvent(event))
    {
        /* Prevent scrolling the page */
        event.setAccepted();
        return;
    }
}

void AdminApp::textInputEvent(TextInputEvent& event)
{
    if (m_imgui.handleTextInputEvent(event))
        return;
}


