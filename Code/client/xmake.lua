
target("client")
    set_kind("binary")
    set_group("Client")
    add_includedirs(
        "../tp_process",
        "../../Libraries/TiltedUI/Code/ui_process/include/",
        "../../Libraries/TiltedUI/ThirdParty/CEF/")
    add_headerfiles("../tp_process/*.h")
    add_files("../tp_process/*.cpp")
    add_deps("UiProcess")
    add_packages("tiltedcore", "hopscotch-map")