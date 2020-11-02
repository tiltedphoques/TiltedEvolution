#pragma once

#include <Stl.hpp>
#include <cstdint>

struct AnimationGraphDescriptor
{
    TiltedPhoques::Vector<uint32_t> BooleanLookUpTable;
    TiltedPhoques::Vector<uint32_t> FloatLookupTable;
    TiltedPhoques::Vector<uint32_t> IntegerLookupTable;
};
