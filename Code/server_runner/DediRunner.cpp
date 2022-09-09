
#include "DediRunner.h"

#include <console/CommandSettingsProvider.h>

#include <chrono>
#include <iostream>
#include <base/threading/ThreadUtils.h>

namespace
{
constexpr char kSettingsFileName[] =
#if TARGET_ST
    "STServer.ini"
#elif TARGET_FT
    "FTServer.ini"
#else
    "Server.ini"
#endif
    ;

DediRunner* s_pRunner{nullptr};
} // namespace

// imports
GS_IMPORT TiltedPhoques::UniquePtr<IGameServerInstance> CreateGameServer(Console::ConsoleRegistry& conReg, const std::function<void()>& aCallback);
// needs to be global
Console::Setting bConsole{"bConsole", "Enable the console", true};

DediRunner* GetDediRunner() noexcept
{
    return s_pRunner;
}

DediRunner::DediRunner(int argc, char** argv)
: m_console(KCompilerStopThisBullshit)
{
    s_pRunner = this;

    uv_loop_init(&m_loop);

    m_pServerInstance = std::move(CreateGameServer(m_console, [this, argc, argv]() { LoadSettings(argc, argv); }));

    // it is here for now..
    m_pServerInstance->Initialize();
    SaveSettingsToIni(m_console, m_SettingsPath);
}

DediRunner::~DediRunner()
{
    if (m_useIni)
        SaveSettingsToIni(m_console, m_SettingsPath);
    uv_loop_close(&m_loop);
}

void DediRunner::LoadSettings(int argc, char** argv)
{
    // If we have line args load, don't use the ini
    if (argc > 1)
    {
        LoadSettingsFromCommand(m_console, argc, argv);
    }
    else
    {
        m_useIni = true;
        m_SettingsPath = fs::current_path() / kConfigPathName / kSettingsFileName;
        if (!exists(m_SettingsPath))
        {
            // there is a bug in here... waiting to be found
            // since we dont register our settings till later, so the server settings might be... missing??
            create_directory(fs::current_path() / kConfigPathName);
            SaveSettingsToIni(m_console, m_SettingsPath);
            return;
        }
        LoadSettingsFromIni(m_console, m_SettingsPath);
    }
}

struct Context
{
    TiltedPhoques::String data;
    DediRunner* instance;
};

void DediRunner::ReadStdin(uv_stream_t* apStream, ssize_t aRead, const uv_buf_t* acpBuffer)
{
    auto* ctx = static_cast<Context*>(apStream->data);

    if (aRead < 0)
    {
        if (aRead == UV_EOF)
            uv_close(reinterpret_cast<uv_handle_t*>(&apStream), nullptr);
    }
    else if (aRead > 0)
    {
        for (auto i = 0; i < aRead; ++i)
        {
            if (acpBuffer->base[i] == '\n')
            {
                ctx->instance->HandleConsole(ctx->data);
                ctx->data = "";
            }
            else
                ctx->data += acpBuffer->base[i];
        }
    }

    // OK to free buffer as write_data copies it.
    if (acpBuffer->base)
        TiltedPhoques::Allocator::GetDefault()->Free(acpBuffer->base);
}

void DediRunner::AllocateBuffer(uv_handle_t* apHandle, size_t aSuggestedSize, uv_buf_t* apBuffer)
{
    *apBuffer = uv_buf_init(static_cast<char*>(TiltedPhoques::Allocator::GetDefault()->Allocate(aSuggestedSize)), static_cast<uint32_t>(aSuggestedSize));
}

void DediRunner::PrintExecutorArrowHack()
{
    // Force:
    // This is a hack to get the executor arrow.
    // If you find a way to do this through the ConOut log channel
    // please let me know (The issue is the forced formatting for that channel and the forced null termination)
    //fmt::print(">>>");
}

void DediRunner::RunGSThread()
{
    while (m_pServerInstance->IsListening())
    {
        m_pServerInstance->Update();
        if (bConsole)
        {
            uv_run(&m_loop, UV_RUN_NOWAIT);
            if (m_console.Update())
                PrintExecutorArrowHack();
        }
    }
}


void DediRunner::StartTerminalIO()
{
    spdlog::get("ConOut")->info("Server started, type /help for a list of commands.");
    PrintExecutorArrowHack();

    uv_tty_init(&m_loop, &m_tty, 0, 1);
    uv_tty_set_mode(&m_tty, UV_TTY_MODE_NORMAL);

    static Context ctx;
    ctx.instance = this;

    m_tty.data = &ctx;

    uv_read_start(reinterpret_cast<uv_stream_t*>(&m_tty), AllocateBuffer, ReadStdin);
}

void DediRunner::RequestKill()
{
    m_pServerInstance->Shutdown();

#if defined(_WIN32)
    // work around Control Handler exception (Control-C) being set
    // https://cdn.discordapp.com/attachments/675107843573022779/941772837339930674/unknown.png
    // being set.
    if (IsDebuggerPresent())
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(300ms);
    }
#endif
}

void DediRunner::HandleConsole(const TiltedPhoques::String& acCommand)
{
    using exr = Console::ConsoleRegistry::ExecutionResult;

    exr r = m_console.TryExecuteCommand(acCommand);

    PrintExecutorArrowHack();

    if (r == exr::kDirty && m_useIni)
        SaveSettingsToIni(m_console, m_SettingsPath);
}
