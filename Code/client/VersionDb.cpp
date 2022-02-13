#include "VersionDb.h"

VersionDb& VersionDb::Get()
{
    static VersionDb s_db;
    return s_db;
}
