#pragma once

namespace Scaleform::Render
{
// NOTE: Scale and AspectRatio variables have been removed from HW viewport
//       they should be applied as a part of Flash scaling by ViewMatrix,
//       which can be a part of RootNode and converts Flash coordinate system
//       into the pixel-based one used by Render2D.
struct Viewport
{
    int BufferWidth;
    int BufferHeight;
    int Left;
    int Top;
    int Width;
    int Height;
    int ScissorLeft;
    int ScissorTop;
    int ScissorWidth;
    int ScissorHeight;

    enum
    {
        View_IsRenderTexture = 1,

        // Support blending into the destination through a render target texture.
        // Only blend/alpha and add modes are supported. Render the texture with
        // source blend factor 1, dest blend factor 1-Sa.
        View_AlphaComposite = 2,
        View_UseScissorRect = 4,

        // Don't set viewport/scissor state on device. Currently only supported by some
        // Renderer implementations.
        View_NoSetState = 8,

        // Defines orientation of the viewport coordinate system within the
        // buffer. This means that BufferWidth/Height remain the same for both orientations,
        // but Top, Left, Width, Height (ans scissor coordinates) change.
        // So, to rotate 480x960 view to landscape, would specify:
        // { BufferWidth,Height = 480,960; Left,Top = 0,0; Width,Height = 960,480, Flags = View_Orientation_R90 }
        View_Orientation_Normal = 0x00,
        View_Orientation_R90 = 0x10,
        View_Orientation_180 = 0x20,
        View_Orientation_L90 = 0x30,
        View_Orientation_Mask = 0x30,

        // For stereo in display hardware only; uses the same size buffer but half for each eye.
        View_Stereo_SplitV = 0x40,
        View_Stereo_SplitH = 0x80,
        View_Stereo_AnySplit = 0xc0,

        View_RenderTextureAlpha = View_IsRenderTexture | View_AlphaComposite,

        // Hal-specific flags should start here
        View_FirstHalFlag = 0x100
    };
    uint32_t Flags;

    void SetScissorRect(int scleft, int sctop, int scw, int sch)
    {
        ScissorLeft = scleft;
        ScissorTop = sctop;
        ScissorWidth = scw;
        ScissorHeight = sch;
        Flags |= View_UseScissorRect;
    }
};
} // namespace Scaleform::Render
