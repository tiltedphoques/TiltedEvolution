ScriptName SkyrimTogetherPlayerAliasScript extends ReferenceAlias  

SkyrimTogetherVerifyLaunchScript Property VerifyLaunchScript Auto

Event OnPlayerLoadGame()
    VerifyLaunchScript.VerifyLaunch()
EndEvent
