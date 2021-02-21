-- TODO (Force): please align into namespace
lib_dir = os.curdir()
directx_dir = lib_dir .. "/DXSDK"

add_requires(
    "tiltedcore",
    "spdlog",
    "discord",
    "imgui")

-- all libraries listed here are under frequent change, and are thus not quired via
-- the xmake package manager, we intend to change this in the future though.

if is_plat("windows") then
	includes("./TiltedUI")
	includes("./TiltedReverse")
	includes("./TiltedHooks")
end

includes("./TiltedScript")
includes("./TiltedConnect")