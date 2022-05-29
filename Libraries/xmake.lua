-- TODO (Force): please align into namespace
lib_dir = os.curdir()
directx_dir = lib_dir .. "/DXSDK"

if is_plat("windows") then
	includes("./TiltedUI")
	includes("./TiltedReverse")
	includes("./TiltedHooks")
end

includes("./TiltedConnect")
