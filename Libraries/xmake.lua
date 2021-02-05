--import("net.http")

-- TODO (Force): please align into namespace
lib_dir = os.curdir()
directx_dir = lib_dir .. "/DXSDK"

-- direct library deps
add_requires("tiltedcore")

-- install third party dependencies

-- all libraries listed here are under frequent change, and are not
-- aquired via the xmake package manager
-- this may change in the future though
includes("./TiltedScript")
includes("./TiltedUI")
includes("./TiltedHooks")
includes("./TiltedReverse")
includes("./TiltedConnect")