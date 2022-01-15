target("Console")
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
