#include "ScriptLoader.h"

namespace fs = std::filesystem;

namespace ScriptLoader
{
void LoadScript(TiltedPhoques::String aPath)
{

}

void LoadScripts()
{
    auto dir = fs::current_path() / "Scripts"; //< Keep upper case to match the game's file system

    TiltedPhoques::Vector<TiltedPhoques::String> files{};
}
}
