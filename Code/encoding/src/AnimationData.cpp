#if TP_SKYRIM

#include <AnimationData.h>

std::array<uint32_t, AnimationData::kBooleanCount> AnimationData::s_booleanLookUpTable{
    129,
    41 ,
    205 ,
    186 ,
    130 ,
    120 ,
    76 ,
    67 ,
    68 ,
    52 ,
    21 ,
    25 ,
    51 ,
    70 ,
    71 ,
    72 ,
    73 ,
    75 ,
    80 ,
    81 ,
    82 ,
    89 ,
    90 ,
    92 ,
    93 ,
    98 ,
    108 ,
    116 ,
    121 ,
    123 ,
    125 ,
    126 ,
    137 ,
    151 ,
    152 ,
    164 ,
    169 ,
    177 ,
    185 ,
    198 ,
    200 ,
    202 ,
    206 ,
    210 ,
    211 ,
    212 ,
    214 ,
    215 ,
    85 ,
    36 ,
    128 ,
    83 ,
    84 ,
    110 ,
    111 ,
    291 ,
    165 ,
    3 ,
    255 ,
    48 ,
    112 ,
    37 ,
    171 ,
    204
};

static_assert(AnimationData::kBooleanCount <= 64);

std::array<uint32_t, AnimationData::kFloatCount> AnimationData::s_floatLookupTable{
    kSpeed,
    kDirection,
    kSpeedSampled,
    kweapAdj
};

std::array<uint32_t, AnimationData::kIntegerCount> AnimationData::s_integerLookupTable{
    kTurnDelta,
    kiRightHandEquipped,
    kiLeftHandEquipped,
    i1HMState,
    kiState,
    kiLeftHandType,
    kiRightHandType
};

#endif

