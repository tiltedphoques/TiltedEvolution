#include "ESLoader.h"
#include <Records/REFR.h>

int main(int argc, char** argv)
{
    ESLoader loader("Data\\");

    UniquePtr<RecordCollection> pCollection = loader.BuildRecordCollection();

    spdlog::info("{:X}", pCollection->GetFormType(0x13480));

    spdlog::info("Build file list succeeded.");

    return 0;
}

