
target("ImmersiveElf")
    set_basename("EarlyLoad")
    set_kind("shared")
    set_group("Client")
    add_includedirs(
        ".",
        "../../Libraries/")
    add_headerfiles("**.h")
    add_files(
        "**.cpp")