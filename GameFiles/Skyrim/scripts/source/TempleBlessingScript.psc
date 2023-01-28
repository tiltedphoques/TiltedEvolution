Scriptname TempleBlessingScript extends ObjectReference Conditional  

Spell Property TempleBlessing  Auto  

Event OnActivate(ObjectReference akActionRef)

	Actor actorRef = akActionRef as Actor
	if SkyrimTogetherUtils.IsRemotePlayer(actorRef)
		return
	endif

	TempleBlessing.Cast(akActionRef, akActionRef)
	if akActionRef == Game.GetPlayer()
		AltarRemoveMsg.Show()
		BlessingMessage.Show()
	endif

EndEvent

Message Property BlessingMessage  Auto  

Message Property AltarRemoveMsg  Auto  
