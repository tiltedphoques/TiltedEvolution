add_requires("glm")

if is_plat("windows") then
    includes("client")
    includes("immersive_launcher")
    includes("tp_process")
    includes("admin")
end

includes("common")
includes("admin_protocol")
includes("server")
includes("encoding")
includes("tests")
includes("es_loader")
