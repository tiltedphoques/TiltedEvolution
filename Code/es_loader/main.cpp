#include "ESLoader.h"

int main(int argc, char** argv)
{
    uint32_t* ints = new uint32_t[4];
    ints[0] = 0x50555247;
    ints[1] = 0x8;
    ints[2] = 0x41414141;
    ints[2] = 0x42424242;
    Buffer buffer;
    buffer.Resize(0x10);
    memcpy(buffer.GetWriteData(), ints, 0x10);
    Buffer::Reader reader(&buffer);
    uint32_t type = 0;
    reader.ReadBytes(reinterpret_cast<uint8_t*>(&type), 4);
    uint32_t size = 0;
    reader.ReadBytes(reinterpret_cast<uint8_t*>(&size), 4);

    //ESLoader loader("Data\\");

    spdlog::info("Build file list succeeded.");

    return 0;
}

