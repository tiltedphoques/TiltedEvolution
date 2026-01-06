Scriptname ResourceFurnitureScript extends ObjectReference  Conditional
{script for furniture which the player can use to get resources}


formlist Property requiredItemList Auto  
{required for player to use - optional}

Message Property FailureMessage Auto  
{Message to say why you can't use this without RequiredWeapon}

MiscObject Property Resource Auto  
{what you get from using this furniture}

int Property ResourceCount = 1 Auto
{how many resources you get per use}

int property MaxResourcePerActivation = 6 auto
{How many times can this object be used before the player has to re-activate?}

int counter
; count up how many resources have been gathered on this go.

faction property CurrentFollowerFaction auto
{Used to handle player followers using the furniture object}

objectReference property NPCfollower auto hidden
{hidden property to track followers who used this}

Event OnLoad()
	BlockActivation(true)
endEvent

Event OnUnload()
	; safety measure
	UnregisterForEvents(game.getplayer())
	if NPCfollower
		UnregisterForEvents(NPCfollower)
	endif
endEvent

auto STATE normal
Event OnActivate(ObjectReference akActionRef)
	Actor actorRef = akActionRef as Actor
	if SkyrimTogetherUtils.IsRemotePlayer(actorRef)
		return
	endif

	if akActionRef == Game.GetPlayer()
		gotoState("busy")
; 		debug.trace(self + "OnActivate")
		if akActionRef == Game.GetPlayer()  || (akActionRef as actor).isInFaction(CurrentFollowerFaction)
; 			debug.trace("akActionRef is either player or a follower")
			if (akActionRef as actor) != game.getPlayer()
; 				debug.trace("It's a follower - store in NPCfollower property")
				; if not the player, must be the follower
				NPCfollower = akActionRef
			endif
			bool allowActivation = true
			; check if player has required item
			if requiredItemList
				if akActionRef.GetItemCount(requiredItemList) == 0
					if akActionRef == game.getPlayer()
						; only require the axe item for the player
						allowActivation = false
; 						debug.trace("allowActivation = "+allowActivation)
						FailureMessage.Show()
					endif
				endif
			endif

			if allowActivation
				RegisterForEvents(akActionRef)
; 				debug.trace(self + "player/follower activation START")
				Activate(akActionRef, true)
; 				debug.trace(self + "player/follower activation END")
			endif
		else
; 			;debug.trace(self + "non-follower NPC activation START")
			; just activate it
			Activate(akActionRef, true)
; 			;debug.trace(self + "non-follower NPC activation END")
		endif
		gotoState("normal") 
	endif
endEvent
endState

STATE busy
	; do nothing
endState

Event OnAnimationEvent(ObjectReference akSource, string asEventName)
; 	debug.trace(self + ": animation event received=" + asEventName)
	if asEventName == "AddToInventory"
		akSource.AddItem(Resource, ResourceCount)
		; increment counter by however many items we just received
; 		debug.trace("Pre-add counter = "+counter)
		counter = (counter + resourceCount)
; 		debug.trace("Post-add counter = "+counter)
		if counter >= MaxResourcePerActivation
			; if we've bagged our limit, kick the player out.  Reset timer for next activation
; 			debug.trace("Woodpile - player has gotten "+counter+" logs this go.  Kicking out.")
			counter = 0
			(akSource as actor).PlayIdle(IdleWoodchopExit)
			unregisterForEvents(akSource)
		endif
	elseif asEventName == "IdleFurnitureExit"
; 		debug.trace("Resource Object Unregistering: "+self)
		; reset the counter if I exit manually
		counter = 0
		UnregisterForEvents(akSource)
	endif
endEvent

bool isRegisteredForEvents = false

function RegisterForEvents(objectReference whoToRegister)
	; centralize this
	isRegisteredForEvents = true
	RegisterForAnimationEvent(whoToRegister, "AddToInventory")
	RegisterForAnimationEvent(whoToRegister, "SoundPlay . NPCHumanWoodChop")
	RegisterForAnimationEvent(whoToRegister, "IdleFurnitureExit")
endFunction

function UnregisterForEvents(objectReference whoToUnregister)
	; centralize this
	
	; It is perfectly safe to unregister for events you never registered for, however
	; this function is called as part of OnUnload, and if this object isn't persistent
	; then it may be deleted by the time OnUnload runs, and these function calls will
	; fail. Since RegisterForAnimationEvent persists us, we know it will be safe to
	; call Unregister if we've previously Registered, even if called as a part of
	; OnUnload
	if isRegisteredForEvents
		isRegisteredForEvents = false
		UnRegisterForAnimationEvent(whoToUnregister, "AddToInventory")
		UnRegisterForAnimationEvent(whoToUnregister, "IdleFurnitureExit")
	endif
endFunction

Idle Property IdleWoodchopExit  Auto  
