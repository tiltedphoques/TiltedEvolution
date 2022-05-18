#pragma once

class CrashHandler
{
  public:
    CrashHandler();
    ~CrashHandler();

    static void RemovePreviousDump(std::filesystem::path path);
};
