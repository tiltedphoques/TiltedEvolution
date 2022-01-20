#include "ESLoader.h"
#include <Records/REFR.h>

int main(int argc, char** argv)
{
    ESLoader loader("Data\\");

    RecordCollection collection = loader.BuildRecordCollection();

    spdlog::info("Build file list succeeded.");

    return 0;
}

