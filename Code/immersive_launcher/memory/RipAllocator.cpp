
// Deliberately in no namespace...

#include <cstdint>

extern char highrip[];
static char* globalPointer = &highrip[0];

void* RipAllocateN(size_t blockLength)
{
    void* p = static_cast<void*>(globalPointer);
    globalPointer += blockLength;
    return p;
}
