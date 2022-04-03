#pragma once

#include <Components/TESModel.h>
#include <Components/TESTexture.h>
#include <Forms/TESForm.h>

class BGSVolumetricLighting;

class TESTexture1024 : public TESTexture
{
  public:
    virtual ~TESTexture1024() = default;
};
static_assert(sizeof(TESTexture1024) == 0x10);

struct Color
{
    std::uint8_t red;   // 0
    std::uint8_t green; // 1
    std::uint8_t blue;  // 2
    std::uint8_t alpha; // 3
};

class BGSDirectionalAmbientLightingColors
{
  public:
    struct Directional
    {
      public:
        template <class T> struct MaxMin
        {
          public:
            // members
            T max; // 0
            T min; // ?
        };
        static_assert(sizeof(MaxMin<Color>) == 0x8);

        // members
        MaxMin<Color> x; // 00
        MaxMin<Color> y; // 08
        MaxMin<Color> z; // 10
    };
    static_assert(sizeof(Directional) == 0x18);

    // members
    Directional directional; // 00
    Color specular;          // 18
    float fresnelPower;      // 1C
};
static_assert(sizeof(BGSDirectionalAmbientLightingColors) == 0x20);

class TESWeather : public TESForm
{
  public:
    enum
    {
        kTotalLayers = 32
    };

    enum class SoundType
    {
        kDefault = 0,
        kPrecip = 1,
        kWind = 2,
        kThunder = 3
    };

    enum class WeatherDataFlag
    {
        kNone = 0,
        kPleasant = 1 << 0,
        kCloudy = 1 << 1,
        kRainy = 1 << 2,
        kSnow = 1 << 3,
        kPermAurora = 1 << 4,
        kAuroraFollowsSun = 1 << 5
    };

    struct ColorTimes
    {
        enum ColorTime : uint32_t
        {
            kSunrise = 0,
            kDay,
            kSunset,
            kNight,

            kTotal
        };
    };
    using ColorTime = ColorTimes::ColorTime;

    struct ColorTypes
    {
        enum
        {
            kSkyUpper = 0,
            kFogNear,
            kUnknown,
            kAmbient,
            kSunlight,
            kSun,
            kStars,
            kSkyLower,
            kHorizon,
            kEffectLighting,
            kCloudLODDiffuse,
            kCloudLODAmbient,
            kFogFar,
            kSkyStatics,
            kWaterMultiplier,
            kSunGlare,
            kMoonGlare,

            kTotal
        };
    };

    struct RecordFlags
    {
        enum RecordFlag : uint32_t
        {
            kDeleted = 1 << 5,
            kIgnored = 1 << 12
        };
    };

    struct Data // DATA
    {
      public:
        struct Color3
        {
          public:
            // members
            int8_t red;   // 0
            int8_t green; // 1
            int8_t blue;  // 2
        };
        static_assert(sizeof(Color3) == 0x3);

        // members
        int8_t windSpeed;                   // 00
        int8_t unk01;                       // 01
        int8_t unk02;                       // 02
        int8_t transDelta;                  // 03
        int8_t sunGlare;                    // 04
        int8_t sunDamage;                   // 05
        int8_t precipitationBeginFadeIn;    // 06
        int8_t precipitationEndFadeOut;     // 07
        int8_t thunderLightningBeginFadeIn; // 08
        int8_t thunderLightningEndFadeOut;  // 09
        int8_t thunderLightningFrequency;   // 0A
        uint8_t flags;                      // 0B
        Color3 lightningColor;              // 0C
        int8_t visualEffectBegin;           // 0F
        int8_t visualEffectEnd;             // 10
        int8_t windDirection;               // 11
        int8_t windDirectionRange;          // 12
        int8_t unk13;                       // 13
    };
    static_assert(sizeof(Data) == 0x14);

    struct FogData // FNAM
    {
      public:
        // members
        float dayNear;    // 00
        float dayFar;     // 04
        float nightNear;  // 08
        float nightFar;   // 0C
        float dayPower;   // 10
        float nightPower; // 14
        float dayMax;     // 18
        float nightMax;   // 1C
    };
    static_assert(sizeof(FogData) == 0x20);

    struct WeatherSoundList : public GameList<void*>
    {
    };

    ~TESWeather() override = 0;

    // members
    TESTexture1024 cloudTextures[kTotalLayers];                                              // 020 - 00TX - L0TX
    int8_t cloudLayerSpeedY[kTotalLayers];                                                   // 220 - RNAM
    int8_t cloudLayerSpeedX[kTotalLayers];                                                   // 240 - QNAM
    Color cloudColorData[kTotalLayers][ColorTime::kTotal];                                   // 260 - PNAM
    float cloudAlpha[kTotalLayers][ColorTime::kTotal];                                       // 460 - JNAM
    uint32_t cloudLayerDisabledBits;                                                         // 660 - NAM1 - bitfield
    Data data;                                                                               // 664 - DATA
    FogData fogData;                                                                         // 678 - FNAM
    Color colorData[ColorTypes::kTotal][ColorTime::kTotal];                                  // 698 - NAM0
    WeatherSoundList sounds;                                                                 // 7A8
    GameArray<void*> skyStatics;                                                             // 7B8
    uint32_t numCloudLayers;                                                                 // 7D0 - LNAM
    uint32_t pad7D4;                                                                         // 7D4
    void* imageSpaces[ColorTime::kTotal];                                                    // 7D8 - IMSP
    BGSDirectionalAmbientLightingColors directionalAmbientLightingColors[ColorTime::kTotal]; // 7F8
    TESModel aurora;                                                                         // 878
    void* sunGlareLensFlare;                                                                 // 8A0
    BGSVolumetricLighting* volumetricLighting[ColorTime::kTotal];                            // 8A8 - HNAM
    void* precipitationData;                                                                 // 8C8 - MNAM
    void* referenceEffect;                                                                   // 8D0 - NNAMf
};
static_assert(sizeof(TESWeather) == 0x8D8);
