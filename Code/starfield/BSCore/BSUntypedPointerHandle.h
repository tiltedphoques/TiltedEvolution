template <uint32_t FREE_LIST_BITS = 21, uint32_t AGE_SHIFT = 5> struct BSUntypedPointerHandle
{
    uint32_t iBits = 0;
};

template <class T, class THandle = BSUntypedPointerHandle<>> struct BSPointerHandle
{
    BSPointerHandle() = default;

    operator bool() const { return handle.iBits != 0; }

    THandle handle{};
};
