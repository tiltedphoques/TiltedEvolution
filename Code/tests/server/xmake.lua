target("TPServerTests")
    set_kind("binary")
    set_group("Tests")
    add_includedirs("../../server", "../../../Libraries")
    set_pcxxheader("../../server/Pch.h")
    add_files("../main.cpp", "*.cpp", "../../server/Services/ObjectLifecycle.cpp")
    add_deps("SkyrimEncoding", "CommonLib", "Console", "TiltedConnect")
    add_packages(
        "tiltedcore",
        "hopscotch-map",
        "catch2",
        "glm",
        "entt",
        "spdlog",
        "gamenetworkingsockets",
        "lua",
        "sol2")
