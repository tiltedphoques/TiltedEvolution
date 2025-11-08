ScriptName SkyrimTogetherVerifyLaunchScript extends Quest  

; Not bound (not implemented) native functions return default values dictated by their return type. 
; `DidLaunchSkyrimTogether()` will return `false` in the base game, however launching 
; SkyrimTogether.exe will make it return `true` because we bind the function (see Misc/BSScript.cpp)

bool Function DidLaunchSkyrimTogether() global native

Event OnInit()
    VerifyLaunch()
EndEvent

Function VerifyLaunch()
    If (!DidLaunchSkyrimTogether())
        Utility.Wait(1)
        Debug.MessageBox("Skyrim Together Error\n\n" \
                       + "Skyrim Together is not running!\n" \
                       + "To play Skyrim Together and access multiplayer features, " \
                       + "launch SkyrimTogether.exe located in 'Skyrim Special Edition\\Data\\SkyrimTogetherReborn'")
    EndIf
EndFunction
