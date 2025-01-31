#pragma once

namespace ShaderFixes
{
// Constants
namespace SetupAndDrawPassConstants
{
constexpr std::uint32_t RAW_FLAG_RIM_LIGHTING = 1 << 11;
constexpr std::uint32_t RAW_FLAG_DO_ALPHA_TEST = 1 << 20;
constexpr std::uint32_t RAW_TECHNIQUE_EYE = 16;
constexpr std::uint32_t RAW_TECHNIQUE_MULTILAYERPARALLAX = 11;
constexpr std::uint32_t RAW_TECHNIQUE_ENVMAP = 1;
constexpr std::uint32_t RAW_TECHNIQUE_PARALLAX = 3;
}

struct BSRenderPass
{
    const static std::int32_t MaxLightInArrayC = 16;

    void* m_Shader;
    void* m_Property;
    void* m_Geometry;
    std::uint32_t m_TechniqueID;
    std::uint8_t Byte1C;
    std::uint8_t Byte1D;
    struct
    {
        std::uint8_t Index : 7;
        bool SingleLevel : 1;
    } m_Lod;
    std::uint8_t m_LightCount;
    uint16_t Word20;
    BSRenderPass* m_Previous;
    BSRenderPass* m_Next;
    void** m_SceneLights;
    std::uint32_t UnkDword40;
};
}
