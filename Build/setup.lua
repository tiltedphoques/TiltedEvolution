premake.extensions.cef_dir = "../Libraries/TiltedUI/ThirdParty/CEF/";
premake.extensions.directx_dir = "../Libraries/DXSDK/";
premake.extensions.discord_dir = "../Libraries/discord_game_sdk/";

if os.isdir(premake.extensions.cef_dir) == false and os.istarget("Windows") == true then
    print("Downloading CEF dependencies...")
    http.download("https://download.skyrim-together.com/ThirdParty.zip", "ThirdParty.zip")
    print("Extracting CEF dependencies...")
    zip.extract("ThirdParty.zip", "../Libraries/TiltedUI/")
    os.remove("ThirdParty.zip")
end

if os.isdir(premake.extensions.directx_dir) == false and os.istarget("Windows") == true then
    print("Downloading DirectX dependencies...")
    http.download("https://download.skyrim-together.com/DXSDK.zip", "DXSDK.zip")
    print("Extracting DirectX dependencies...")
    zip.extract("DXSDK.zip", premake.extensions.directx_dir)
    os.remove("DXSDK.zip")
end

-- blame discord for this
if os.isdir(premake.extensions.discord_dir) == false and os.istarget("Windows") == true then
    print("Downloading Discord Game SDK...")
    http.download("https://dl-game-sdk.discordapp.net/latest/discord_game_sdk.zip", "DSDK.zip")
    print("Extracting Discord Game SDK...")
    zip.extract("DSDK.zip", premake.extensions.discord_dir)
    os.remove("DSDK.zip")
end

local f = io.popen('git symbolic-ref --short -q HEAD', 'r')
local temp = f:read("*a")
f:close()
local git_branch = string.gsub(temp, '\n$', '')

f = io.popen('git rev-parse --short HEAD', 'r')
temp = f:read("*a")
f:close()
local git_commit = string.gsub(temp, '\n$', '')

defines({
    ('GIT_BRANCH="' .. git_branch .. '"'),
    ('GIT_COMMIT="' .. git_commit .. '"'),
})
