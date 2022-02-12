#include "VersionDb.h"

VersionDb& GetVersionDb()
{
    static std::mutex lock;
    std::scoped_lock _{lock};

    static VersionDb db;
    static std::once_flag flag;
    std::call_once(flag, []() { 
        auto result = db.Load(1,6,353,0);
        if (result != true)
        {
            spdlog::error("Failed to load address library!");
            exit(-1);
        }
    });

    return db;
}
