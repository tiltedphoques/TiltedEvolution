
// Deliberately in no namespace...

#include <cstdint>

extern uint8_t highrip[];
static uint8_t* globalPointer = &highrip[0];

void* RipAllocateN(size_t blockLength)
{
    void *p = static_cast<void*>(globalPointer);
    globalPointer += blockLength;
    return p;
}

// TBD
void RipFree(void* apBlock)
{

}
