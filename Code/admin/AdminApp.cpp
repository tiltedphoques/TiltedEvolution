#include "AdminApp.h"

#include <AdminMessages/ServerAdminMessageFactory.h>

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>

AdminApp::AdminApp(const Arguments& arguments)
    : Platform::Application{
          arguments,
          Configuration{}.setTitle("TiltedOnline Server Admin").setWindowFlags(Configuration::WindowFlag::Resizable)}
{
    m_password.resize(1024);
    m_imgui = ImGuiIntegration::Context(Vector2{windowSize()} / dpiScaling(), windowSize(), framebufferSize());

    /* Set up proper blending to be used by ImGui. There's a great chance
       you'll need this exact behavior for the rest of your scene. If not, set
       this only for the drawFrame() call. */
    GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add, GL::Renderer::BlendEquation::Add);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
                                   GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    auto handlerGenerator = [this](auto& x) {
        using T = typename std::remove_reference_t<decltype(x)>::Type;

        m_messageHandlers[T::Opcode] = [this](UniquePtr<ServerAdminMessage>& apMessage) {
            const auto pRealMessage = TiltedPhoques::CastUnique<T>(std::move(apMessage));
            HandleMessage(*pRealMessage);
        };

        return false;
    };

    ServerAdminMessageFactory::Visit(handlerGenerator);

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
    if (m_state != ConnectionState::kConnected)
    {
        ImGui::SetNextWindowSize(ImVec2(600, 150));
        ImGui::SetNextWindowPos(ImVec2(windowSize().x() / 2, 200), 0, ImVec2(0.5f, 0.f));
        ImGui::Begin("Online", nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar |
                         ImGuiWindowFlags_MenuBar);

        if (m_state == ConnectionState::kConnecting)
            ImGui::Text("Please wait...");
        else if (m_state == ConnectionState::kNone)
        {
            static char s_endpoint[1024] = "127.0.0.1:10578";

            ImGui::InputText("Endpoint", s_endpoint, std::size(s_endpoint));
            ImGui::InputText("Password", m_password.data(), std::size(m_password), ImGuiInputTextFlags_Password);

            if (ImGui::Button("Connect"))
            {
                Connect(s_endpoint);
                m_state = ConnectionState::kConnecting;
            }
        }

        ImGui::End();
    }

    if (m_state == ConnectionState::kConnected)
        drawServerUi();

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
    Update();
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


void AdminApp::drawServerUi()
{
    m_overlay.Update(*this);
}

