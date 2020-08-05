cefDir = "../Libraries/TiltedUI/ThirdParty/CEF/";
dsdkDir = "../Libraries/discord_game_sdk/";

if os.isdir(cefDir) == false and os.istarget("Windows") == true then
    print("Downloading CEF dependencies...")

    http.download("https://download.skyrim-together.com/ThirdParty.zip", "ThirdParty.zip")
   
    print("Extracting CEF dependencies...")
    zip.extract("ThirdParty.zip", "../Libraries/TiltedUI/")
    os.remove("ThirdParty.zip")
end

-- blame discord for this
if os.isdir(dsdkDir) == false and os.istarget("Windows") == true then
    print("Downloading Discord Game SDK...")
    http.download("https://dl-game-sdk.discordapp.net/latest/discord_game_sdk.zip", "DSDK.zip")
    print("Extracting Discord Game SDK...")
    zip.extract("DSDK.zip", dsdkDir)
    os.remove("DSDK.zip")
end
