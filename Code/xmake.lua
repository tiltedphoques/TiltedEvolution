
-- hack
function forceincludes(path)
if is_plat("windows") then
    add_cxxflags("/FI\"" .. path .. "\"", { force=true })
else
    add_cxxflags("-include " .. path, { force=true })
end
end

includes("./common")
includes("./client")
includes("./server")
includes("./tp_process")
includes("./encoding")
--includes("./tests")
