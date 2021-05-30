add_requires("vcpkg::magnum")
add_requires("vcpkg::sdl2")
add_requires("vcpkg::magnum-integration[imgui]")
add_requires("vcpkg::corrade")

target("Admin")
    set_kind("binary")
    set_group("Client")
    set_basename("TiltedAdmin")
    set_symbols("debug", "hidden")
    add_includedirs(
        ".",
        "../",
        "../../Libraries/")
    add_headerfiles("**.h")
    add_files(
        "**.cpp",
        "admin.rc")
    add_deps("Common", "AdminProtocol")

    if name == "SkyrimTogetherServer" then
        add_deps("SkyrimEncoding")
    end
    if name == "FalloutTogetherServer" then
        add_deps("FalloutEncoding")
    end

    if is_plat("windows") then
        add_syslinks("opengl32", "Shell32", "Gdi32", "Winmm", "Ole32", "version", "OleAut32", "Setupapi")
    end

    add_packages(
        "tiltedcore",
        "tiltedconnect",
        "spdlog",
        "hopscotch-map",
        "glm",
        "vcpkg::corrade",
        "vcpkg::magnum",
        "vcpkg::magnum-integration[imgui]",
        "imgui",
        "vcpkg::sdl2")
