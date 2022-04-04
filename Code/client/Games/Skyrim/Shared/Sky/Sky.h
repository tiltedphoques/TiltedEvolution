#pragma once

#include "NetImmerse/NiNode.h"
#include "NetImmerse/NiPointer.h"

#include "Shared/Sky/Atmosphere.h"
#include "Shared/Sky/Clouds.h"
#include "Shared/Sky/Moon.h"
#include "Shared/Sky/Precipitation.h"
#include "Shared/Sky/Stars.h"
#include "Shared/Sky/Sun.h"

#include <Shared/TESForms/World/TESClimate.h>
#include <Shared/TESForms/World/TESWeather.h>

class ReferenceEffect;
class SkySound;
class BGSLightingTemplate;

class BSFogProperty;
class BSMultiBoundNode;
class TESWeather;
class TESRegion;
class TESImageSpace;
class TESGlobal;

// todo: find new home for this.
struct ReferenceEffectController
{
    virtual ~ReferenceEffectController() = default;
};

struct SkyEffectController : public ReferenceEffectController
{
};

struct ImageSpaceModifierInstanceForm;

class Sky
{
    friend class Stars;
    friend class Moon;
    friend class Clouds;
    friend class Atmosphere;

  public:
    virtual ~Sky() = 0;

    virtual void m1() = 0;

    enum SKY_MODE : int
    {
        SM_NONE = 0x0,
        SM_INTERIOR = 0x1,
        SM_SKYDOME_ONLY = 0x2,
        SM_FULL = 0x3,
        SM_COUNT = 0x4,
    };

    enum SKY_FLAGS : int
    {
        USE_ACCEL = 8,
        FAST_TRAVEL = 0x10,
        HIDE_SKY = 0x80,
        NEXT_ROOM = 0x100,
        UPDATE_WEATHER_DATA = 0x200000,
        USE_CACHED_STATIC_DATA = 0x400000,
        SKIP_TRANSITION = 0x1000000,
    };

    inline bool IsDisabled() const
    {
        return eMode != SM_FULL && eMode != SM_SKYDOME_ONLY;
    }

    SKY_MODE GetSkyMode() const
    {
        return eMode;
    }

    TESClimate* GetCurrentClimate() const noexcept
    {
        return pCurrentClimate;
    }

    TESWeather* GetCurrentWeather() const noexcept
    {
        return pCurrentWeather;
    }

    TESWeather* GetLastWeather() const noexcept
    {
        return pLastWeather;
    }

    TESWeather* GetDefaultWeather() const noexcept
    {
        return pDefaultWeather;
    }

    TESWeather* GetOverrideWeather() const noexcept
    {
        return pOverrideWeather;
    }

    TESRegion* GetCurrentRegion() const noexcept
    {
        return pCurrentRegion;
    }

    void SkipWeatherTransition()
    {
        uiFlags |= SKIP_TRANSITION;
    }

    void ForceNextRoom()
    {
        uiFlags |= NEXT_ROOM;
    }

    float GetCurrentGameHour()
    {
        return fCurrentGameHour;
    }

    float GetLastWeatherUpdate()
    {
        return fLastWeatherUpdate;
    }

    // CurrentWeatherPercent is only greater than one if pLastWeather exists
    // and it indicates the amount of blend between the last and current(future) weather
    float GetCurrentWeatherPercent()
    {
        return fCurrentWeatherPct;
    }

    float GetCurrentWeatherTimeElapsed();

    // When the duration goes over specified length, a new weather is chosen
    float GetCurrentWeatherDuration();

    void SetHideSky(bool abVal);
    void SetFastTravel(bool abOn);

    static Sky* GetInstance();

    void SetWeatherExternal(TESWeather* newWeather, bool overrideWeather, bool updateLastWeather);

  public:
    NiPointer<BSMultiBoundNode> spRoot;
    NiPointer<NiNode> spMoonsRoot;
    NiPointer<NiNode> spAuroraRoot;
    char hAurora3d[8];
    BGSLightingTemplate* pExtLightingOverride;
    BSPointerHandle<TESObjectREFR, BSUntypedPointerHandle<21, 5>> hCurrentRoom;
    BSPointerHandle<TESObjectREFR, BSUntypedPointerHandle<21, 5>> hPreviousRoom;
    float fLightingTransition;
    float fLightingTransitionTimer;
    TESClimate* pCurrentClimate = nullptr;
    TESWeather* pCurrentWeather = nullptr;
    TESWeather* pLastWeather = nullptr;
    TESWeather* pDefaultWeather = nullptr;
    TESWeather* pOverrideWeather = nullptr;
    TESRegion* pCurrentRegion = nullptr;
    Atmosphere* pAtmosphere = nullptr;
    Stars* pStars = nullptr;
    Sun* pSun = nullptr;
    Clouds* pClouds = nullptr;
    Moon* pMasser = nullptr;
    Moon* pSecunda = nullptr;
    Precipitation* pPrecip = nullptr;
    NiColor SkyColor[17]; // 0xA8

#if 0
    NiColor PrevSkyColorA[17];
    NiColor PrevDirAmbColorsA[3][2];
    char PrevSpecTintFres[4 * 4];
    TESImageSpace* pPrevImageSpace = nullptr;
    float fPrevFogDistancesA[8];
    float fPrevFogHeight;
    float fPrevFogPower;
    float fPrevFogClamp;
    float fPrevFogHighDensityScale;
    float fLastExtWetness;
    NiColor WaterFogColor;
    NiColor SunSpecularColor;
    float fWindSpeed = 0.f;
    float fWindAngle = 1.f;
    float fWindTurbulence = 0.2f;
    float fFogDistances[8];
    float fFogHeight;
    float fFogPower;
    float fFogClamp;
    float fFogHighDensityScale;
#endif
    uint32_t unk174;        // 174
    uint64_t unk178;        // 178
    uint64_t unk180;        // 180
    uint64_t unk188;        // 188
    uint64_t unk190;        // 190
    uint64_t unk198;        // 198
    uint32_t unk1A0;        // 1A0
    uint32_t unk1A4;        // 1A4
    float unk1A8;           // 1A8
    float unk1AC;           // 1AC
    float fCurrentGameHour; // 0x1B0
    float fLastWeatherUpdate;
    float fCurrentWeatherPct = 1.0f;
    float fLastWindDirection;
    float fLastWindDirectionRange;
    Sky::SKY_MODE eMode;
    GameList<SkySound*>* pSkySoundList;
    float fFlash;
    uint64_t uiFlashTime;
    uint32_t uiLastMoonPhaseUpdate;
    float fWindowReflectionTimer;
    float fAccelBeginPct;
    uint32_t uiFlags;
    ImageSpaceModifierInstanceForm* pCurrentWeatherImageSpaceMod;
    ImageSpaceModifierInstanceForm* pCurrentWeatherImageSpaceMod2;
    ImageSpaceModifierInstanceForm* pLastWeatherImageSpaceMod;
    ImageSpaceModifierInstanceForm* pLastWeatherImageSpaceMod2;
    NiColor DirectionalAmbientColorsA[3][2];
    NiColor AmbientSpecularTint;
    float fAmbientSpecularFresnel;
    float fAuroraInStart;
    float fAuroraIn;
    float fAuroraOutStart;
    float fAuroraOut;
    // NiPointer<ReferenceEffect> spCurrentReferenceEffect;
    // NiPointer<ReferenceEffect> spLastReferenceEffect;
    SkyEffectController EffectController;
    GameArray<NiPointer<NiTexture>> StoredCloudTexturesA;
    GameArray<NiPointer<NiTexture>> StoredWorldMapCloudTexturesA;
    // GameArray<Sky::SkyStaticRefData> SkyStaticRefDataA;
  private:
    static Sky* pInstance;
};

static_assert(offsetof(Sky, Sky::SkyColor) == 0xA8);
static_assert(offsetof(Sky, Sky::fCurrentGameHour) == 0x1B0);
static_assert(offsetof(Sky, Sky::fCurrentWeatherPct) == 0x1B8);
