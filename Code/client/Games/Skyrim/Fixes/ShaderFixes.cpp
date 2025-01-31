#include <Games/Skyrim/Fixes/ShaderFixes.h>

// Fixes incorporated from SSE Engine Fixes

#if TP_PLATFORM_64
constexpr uintptr_t BSLightingShader_vtbl_addr = 0x1856C58; // ID 255053

TP_THIS_FUNCTION(TSetupAndDrawPass, void, ShaderFixes::BSRenderPass, std::uint32_t, bool, std::uint32_t);
static TSetupAndDrawPass* RealSetupAndDrawPass = nullptr;

void TP_MAKE_THISCALL(HookSetupAndDrawPass, ShaderFixes::BSRenderPass, std::uint32_t technique, bool alphaTest, std::uint32_t renderFlags)
{
    using namespace ShaderFixes::SetupAndDrawPassConstants;
    if (*static_cast<std::uintptr_t*>(apThis->m_Shader) == BSLightingShader_vtbl_addr && alphaTest)
    {
        auto rawTechnique = technique - 0x4800002D;
        auto subIndex = (rawTechnique >> 24) & 0x3F;
        
        if (subIndex != RAW_TECHNIQUE_EYE && 
            subIndex != RAW_TECHNIQUE_ENVMAP && 
            subIndex != RAW_TECHNIQUE_MULTILAYERPARALLAX && 
            subIndex != RAW_TECHNIQUE_PARALLAX)
        {
            technique = technique | RAW_FLAG_DO_ALPHA_TEST;
            apThis->m_TechniqueID = technique;
        }
    }

    return TiltedPhoques::ThisCall(RealSetupAndDrawPass, apThis, technique, alphaTest, renderFlags);
}

void PatchSetupAndDrawPass()
{
    POINTER_SKYRIMSE(TSetupAndDrawPass, s_originalSetupAndDrawPass, 107644);
    RealSetupAndDrawPass = s_originalSetupAndDrawPass.Get();

    auto hookLocation = mem::pointer(RealSetupAndDrawPass);

    struct SetupAndDrawPass_Code : TiltedPhoques::CodeGenerator
    {
        SetupAndDrawPass_Code(mem::pointer apReturnLoc)
        {
            // Original instructions
            mov(rax, rsp);
            push(rdi);
            push(r12);

            // Jump back to original code
            jmp_S(apReturnLoc + 0x6);
        }
    } code(hookLocation);
    
    code.ready();

    TiltedPhoques::Jump(hookLocation, code.getCode());
    TP_HOOK(&RealSetupAndDrawPass, HookSetupAndDrawPass);
}

// Parallax
TP_THIS_FUNCTION(TSetupGeometryParallax, void, BSLightingShader);
static TSetupGeometryParallax* RealSetupGeometryParallax = nullptr;

void PatchParallaxSetupGeometry()
{
    POINTER_SKYRIMSE(TSetupGeometryParallax, s_originalSetupGeometryParallax, 107300);
    RealSetupGeometryParallax = s_originalSetupGeometryParallax.Get();

    auto hookLocation = mem::pointer(RealSetupGeometryParallax) + 0xB5D;

    struct ParallaxSetupGeometry_Code : TiltedPhoques::CodeGenerator
    {
        ParallaxSetupGeometry_Code(mem::pointer apReturnLoc)
        {
            // Original instructions
            test(eax, 0x21C00);
            mov(r9d, 1);
            cmovnz(ecx, r9d);

            // New code for parallax fix
            cmp(dword[rbp + (0x1D0 - 0x210)], 0x3);  // Check if technique is PARALLAX
            cmovz(ecx, r9d);  // Force eye update if parallax

            // Jump back to original code
            jmp_S(apReturnLoc + 0xF);
        }
    } code(hookLocation);
    
    code.ready();

    TiltedPhoques::Jump(hookLocation, code.getCode());
}

static TiltedPhoques::Initializer s_shaderFixesInitializer(
    []()
    {
        PatchSetupAndDrawPass();
        PatchParallaxSetupGeometry();
    });
#endif
