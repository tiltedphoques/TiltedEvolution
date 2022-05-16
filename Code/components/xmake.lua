-- This function defines the core component idoms
function component(name)
  target(name)
    set_kind("static")
    set_group("Components")
    add_configfiles("BuildInfo.h.in")
    add_includedirs(
      ".",
      "../",
      "../../", 
      "../../../build", 
      {public = true})
    add_headerfiles("**.h")
    add_files("**.cpp")
    add_packages(
      "tiltedcore", 
      "hopscotch-map", 
      "gtest",
      "spdlog")
end

-- this isnt fully specified yet.
function unittest(name)
    target(name .. "_Tests")
      set_kind("binary")
      set_group("Tests")
      add_configfiles("BuildInfo.h.in")
      add_includedirs(
        ".",
        "../",
        "../../", 
        "../../../build", 
        {public = true})
      add_headerfiles(
          "**.h")
      add_files(
          "**.cpp",
          "../../TestMain.cpp")
      add_packages(
        "tiltedcore", 
        "hopscotch-map", 
        "gtest",
        "spdlog")
  end

-- List all components required below:
includes("console")
includes("imgui")
includes("es_loader")
includes("crash_handler")
