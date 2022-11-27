#pragma once

#include "Script.h"

class ScriptStore
{
public:
    ScriptStore() = delete;
    ScriptStore(TiltedPhoques::Vector<Script>&& aScripts);

private:
    TiltedPhoques::Vector<Script> m_scripts;
};
