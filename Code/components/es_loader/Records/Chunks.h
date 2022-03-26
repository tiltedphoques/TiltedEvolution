#pragma once

class Record;

namespace Chunks
{

struct ScriptProperty
{
    enum class Type
    {
        INVALID = 0,
        OBJECT = 1,
        WSTRING = 2,
        INT = 3,
        FLOAT = 4,
        BOOL = 5,
        OBJECT_ARRAY = 11,
        STRING_ARRAY = 12,
        INT_ARRAY = 13,
        FLOAT_ARRAY = 14,
        BOOL_ARRAY = 15
    };

    enum class Status
    {
        PROPERTY_EDITED = 1,
        PROPERTY_REMOVED = 3,
    };

    union Data {
        uint32_t m_formId;
        uint32_t m_integer;
        float m_float;
        bool m_boolean;

        struct Str
        {
          const char* data;
          size_t length;
        } m_string {nullptr, 0};
    };

    void ParseValue(Buffer::Reader& aReader, int16_t aObjectFormat, TiltedPhoques::Map<uint8_t, uint32_t>& aParentToFormIdPrefix) noexcept;
    Type GetPropertyType(Type aArrayType) noexcept;

    String m_name;
    Type m_type = Type::INVALID;
    Status m_status = Status::PROPERTY_EDITED;
    Data m_dataSingleValue;
    Vector<Data> m_dataArray;
};

struct Script
{
    String m_name;
    uint8_t m_status;
    uint16_t m_propertyCount;
    Vector<ScriptProperty> m_properties;
};

struct VMAD
{
    VMAD(){};
    VMAD(Buffer::Reader& aReader, TiltedPhoques::Map<uint8_t, uint32_t>& aParentToFormIdPrefix);

    int16_t m_version = 0;
    int16_t m_objectFormat = 0;
    uint16_t m_scriptCount = 0;
    Vector<Script> m_scripts{};
};

struct CNTO
{
    CNTO(){}
    CNTO(Buffer::Reader& aReader);

    uint32_t m_formId{};
    uint32_t m_count{};
};

struct WLST
{
    WLST(){}
    WLST(Buffer::Reader& aReader, TiltedPhoques::Map<uint8_t, uint32_t>& aParentToFormIdPrefix);

    uint32_t m_weatherId{}; // WTHR
    uint32_t m_chance{};
    uint32_t m_globalId{};
};

struct TNAM
{
    TNAM(){}
    TNAM(Buffer::Reader& aReader);

    uint8_t m_sunriseBegin{}; // times 10 minutes
    uint8_t m_sunriseEnd{}; // times 10 minutes
    uint8_t m_sunsetBegin{}; // times 10 minutes
    uint8_t m_sunsetEnd{}; // times 10 minutes
    uint8_t m_volatility{}; // 0-100
    uint8_t m_moons{};
};

struct NAME
{
    NAME(){}
    NAME(Buffer::Reader& aReader, TiltedPhoques::Map<uint8_t, uint32_t>& aParentToFormIdPrefix);

    uint32_t m_baseId = 0;
};

struct DOFT
{
    DOFT(){}
    DOFT(Buffer::Reader& aReader, TiltedPhoques::Map<uint8_t, uint32_t>& aParentToFormIdPrefix);

    uint32_t m_formId = 0;
};

struct ACBS
{
    ACBS(){}
    ACBS(Buffer::Reader& aReader);

    enum ActorFlags
    {
        kFemale = 1 << 0,
        kEssential = 1 << 1,
        kIsChargenFacePreset = 1 << 2,
        kRespawn = 1 << 3,
        kAutoCalcStats = 1 << 4,
        kUnique = 1 << 5,
        kDoesntAffectStealth = 1 << 6,
        kPCLevelMult = 1 << 7,
        kAudioTemplate_m = 1 << 8, // ??
        kProtected = 1 << 9,
        kSummonable = 1 << 10,
        kDoesntBleed = 1 << 11,
        kOwnedOrFollowed_m = 1 << 12, // ??
        kOppositeGenderAnims = 1 << 13,
        kSimpleActor = 1 << 14,
        kLoopedScript_m = 1 << 15, // ??
        kLoopedAudio_m = 1 << 16, // ??
        kGhostOrNonInteractable = 1 << 17,
        kInvulnerable = 1 << 18,
    };

    enum TemplateFlags
    {
        kTraits = 1 << 0,
        kStats = 1 << 1,
        kFactions = 1 << 2,
        kSpells = 1 << 3,
        kAIData = 1 << 4,
        kAIPackages = 1 << 5,
        kUnused = 1 << 6,
        kBaseData = 1 << 7,
        kInventory = 1 << 8,
        kScript = 1 << 9,
        kAIDefPackList = 1 << 10,
        kAttackData = 1 << 11,
        kKeywords = 1 << 12
    };

    [[nodiscard]] bool IsFemale() const { return m_flags & ActorFlags::kFemale; }
    [[nodiscard]] bool IsEssential() const { return m_flags & ActorFlags::kEssential; }
    [[nodiscard]] bool IsChargenFacePreset() const { return m_flags & ActorFlags::kIsChargenFacePreset; }
    [[nodiscard]] bool IsRespawn() const { return m_flags & ActorFlags::kRespawn; }
    [[nodiscard]] bool IsAutoCalcStats() const { return m_flags & ActorFlags::kAutoCalcStats; }
    [[nodiscard]] bool IsUnique() const { return m_flags & ActorFlags::kUnique; }
    [[nodiscard]] bool DoesAffectStealth() const { return !(m_flags & ActorFlags::kDoesntAffectStealth); }
    [[nodiscard]] bool IsPCLevelMult() const { return m_flags & ActorFlags::kPCLevelMult; }
    [[nodiscard]] bool IsAudioTemplate() const { return m_flags & ActorFlags::kAudioTemplate_m; }
    [[nodiscard]] bool IsProtected() const { return m_flags & ActorFlags::kProtected; }
    [[nodiscard]] bool IsSummonable() const { return m_flags & ActorFlags::kSummonable; }
    [[nodiscard]] bool DoesBleed() const { return !(m_flags & ActorFlags::kDoesntBleed); }
    [[nodiscard]] bool IsOwnerOrFollowed() const { return m_flags & ActorFlags::kOwnedOrFollowed_m; }
    [[nodiscard]] bool IsOppositeGenderAnims() const { return m_flags & ActorFlags::kOppositeGenderAnims; }
    [[nodiscard]] bool IsSimpleActor() const { return m_flags & ActorFlags::kSimpleActor; }
    [[nodiscard]] bool IsLoopedScript() const { return m_flags & ActorFlags::kLoopedScript_m; }
    [[nodiscard]] bool IsLoopedAudio() const { return m_flags & ActorFlags::kLoopedAudio_m; }
    [[nodiscard]] bool IsGhostOrNonInteractable() const { return m_flags & ActorFlags::kGhostOrNonInteractable; }
    [[nodiscard]] bool IsInvulnerable() const { return m_flags & ActorFlags::kInvulnerable; }

    uint32_t m_flags = 0;
    int16_t m_magickaOffset = 0;
    int16_t m_staminaOffset = 0;
    uint16_t m_level = 0;
    uint16_t m_calcMinLevel = 0;
    uint16_t m_calcMaxLevel = 0;
    uint16_t m_speedMultiplier = 0;
    uint16_t m_dispositionBase = 0;
    uint16_t m_templateDataFlags = 0;
    int16_t m_healthOffset = 0;
    uint16_t m_bleedoutOverride = 0;
};

static_assert(sizeof(ACBS) == 0x18);

struct MAST
{
    MAST(){}
    MAST(Buffer::Reader& aReader);

    String m_masterName = "";
};

struct MapMarkerData
{
    enum class MARKER_TYPE : uint16_t
    {
        kNone = 0,
        kCity = 1,
        kTown = 2,
        kSettlement = 3,
        kCave = 4,
        kCamp = 5,
        kFort = 6,
        kNordicRuins = 7,
        kDwemerRuin = 8,
        kShipwreck = 9,
        kGrove = 10,
        kLandmark = 11,
        kDragonLair = 12,
        kFarm = 13,
        kWoodMill = 14,
        kMine = 15,
        kImperialCamp = 16,
        kStormcloakCamp = 17,
        kDoomstone = 18,
        kWheatMill = 19,
        kSmelter = 20,
        kStable = 21,
        kImperialTower = 22,
        kClearing = 23,
        kPass = 24,
        kAlter = 25,
        kRock = 26,
        kLighthouse = 27,
        kOrcStronghold = 28,
        kGiantCamp = 29,
        kShack = 30,
        kNordicTower = 31,
        kNordicDwelling = 32,
        kDocks = 33,
        kShrine = 34,
        kRiftenCastle = 35,
        kRiftenCapitol = 36,
        kWindhelmCastle = 37,
        kWindhelmCapitol = 38,
        kWhiterunCastle = 39,
        kWhiterunCapitol = 40,
        kSolitudeCastle = 41,
        kSolitudeCapitol = 42,
        kMarkarthCastle = 43,
        kMarkarthCapitol = 44,
        kWinterholdCastle = 45,
        kWinterholdCapitol = 46,
        kMorthalCastle = 47,
        kMorthalCapitol = 48,
        kFalkreathCastle = 49,
        kFalkreathCapitol = 50,
        kDawnstarCastle = 51,
        kDawnstarCapitol = 52,
        kDLC02_TempleOfMiraak = 53,
        kDLC02_RavenRock = 54,
        kDLC02_BeastStone = 55,
        kDLC02_TelMithryn = 56,
        kDLC02_ToSkyrim = 57,
        kDLC02_ToSolstheim = 58
    };

    enum class MARKER_FLAGS : uint8_t
    {
        NONE = 0,
        VISIBLE = 1 << 0,
        TRAVEL_LOC = 1 << 1,
        HIDDEN = 1 << 2,
        USE_LOC_NAME = 1 << 3,
    };

    bool m_isMarker = false;
    MARKER_TYPE m_marker = MARKER_TYPE::kNone;
    MARKER_FLAGS m_flags = MARKER_FLAGS::NONE;
};

struct TypedValue
{
    enum class TYPE
    {
        BOOL = 0,
        CHAR = 1,
        HEX_CHAR_M = 2,
        INT = 3,
        UINT = 4,
        FLOAT = 5,
        LSTRING = 6,
        RGB = 7,
        RGBA = 8,
        UNK = 9,
    };

    TYPE m_type = TYPE::UNK;

    union {
        uint32_t m_int = 0;
        float m_float;
        bool m_bool;
    };
};

struct WCTR
{
    WCTR()
    {
    }
    WCTR(Buffer::Reader& aReader);

    int16_t m_x;
    int16_t m_y;
};

struct DNAM
{
    DNAM()
    {
    }
    DNAM(Buffer::Reader& aReader);

    float m_landLevel;
    float m_waterLevel;
};

struct NVNM
{
    NVNM()
    {
    }
    NVNM(Buffer::Reader& aReader);

    struct Tri
    {
        int16_t m_vertex0;
        int16_t m_vertex1;
        int16_t m_vertex2;

        int16_t m_edge0;
        int16_t m_edge1;
        int16_t m_edge2;

        int16_t m_coverMarker;
        int16_t m_coverFlags;
    };

    struct Connection
    {
        uint32_t m_unk;
        uint32_t m_navMeshId;
        int16_t tri;
    };

    struct Door
    {
        int16_t tri;
        uint32_t m_unk;
        uint32_t m_doorId;
    };

    uint32_t m_unknown;
    uint32_t m_locactionMarker;
    uint32_t m_worldSpaceId;
    std::optional<uint32_t> m_cellId;
    std::optional<int16_t> m_gridX;
    std::optional<int16_t> m_gridY;
    Vector<glm::vec3> m_vertices;
    Vector<Tri> m_triangles;
    Vector<Connection> m_connections;
    Vector<Door> m_doorTris;
    Vector<int16_t> m_coverTris;
    uint32_t m_divisor;
    glm::vec2 m_maxDistance;
    glm::vec3 m_min;
    glm::vec3 m_max;
};

static_assert(sizeof(glm::vec3) == 12);
static_assert(sizeof(NVNM::Tri) == 16);

} // namespace
