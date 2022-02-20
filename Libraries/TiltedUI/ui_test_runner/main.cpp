
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define UNICODE
#include <windows.h>

#include "GraphicsRenderer.h"

#include <string>
#include <assert.h>

class UITestRunner final : public GraphicsRenderer {
public:
    UITestRunner(int argc, char** argv)
    {
        GraphicsRenderer::Initialize(GetModuleHandleW(nullptr));
    }

    bool Draw()
    {
        bool re = GraphicsRenderer::Run();

        return re;
    }
};


int main(int argc, char** argv) {
    UITestRunner runner(argc, argv);
    while (true)
    {
        if (!runner.Draw()) break;
    }

    return 0;
}
