Scriptname PetFramework_PetQuest extends Quest  Conditional
{Manages the pet's home location, commands, etc.  Some functions are called from the pet actor script.}

PetFramework_ParentQuestScript Property PetFramework_ParentQuest Auto
Message Property PetFramework_PetDismissMessage Auto
Message Property PetFramework_PetMaxReachedMessage Auto
Faction Property PetFramework_PetFollowingFaction Auto
Faction Property PlayerFaction Auto
Faction Property PetFramework_PetFaction Auto
ReferenceAlias Property PetHomeMarker Auto
ReferenceAlias Property PetRefAlias Auto
ReferenceAlias Property PetDynamicHomeMarker Auto
Bool Property MovingTogglePackageOn = False Auto Conditional Hidden ;Hearthfire adoption/move trick - turn on a temporary package

Function MakePetAvailableToPlayer()
{Called when the quest meets the criteria for the pet to be available, i.e. purchase, rescue, whatever scenario.  Without this the pet cannot be talked to or recruited.}
	PetRefAlias.GetActorReference().SetFactionRank(PetFramework_PetFaction, 1)
	;PetRefAlias.GetActorReference().AddToFaction(PlayerFaction)
EndFunction

Function MakePetUnavailableToPlayer()
{If for some reason we want to make the pet unavailable to player, i.e. sell it someone else, etc. this function will do that}
	PetRefAlias.GetActorReference().SetFactionRank(PetFramework_PetFaction, 0)
	;PetRefAlias.GetActorReference().RemoveFromFaction(PlayerFaction)
EndFunction

Function FollowPlayer(Bool snapIntoInteraction = False)
{Called when the player recruits the pet via dialogue.}

	If(PetFramework_ParentQuest.HasMaxPets())
		PetFramework_PetMaxReachedMessage.Show()
	Else
	
		debug.trace("Pet Framework: " + PetRefAlias.GetActorReference() + " setting to following player")
		debug.trace("Pet Framework: " + PetRefAlias.GetActorReference() + " PetFollowingFactionRank: " + PetRefAlias.GetActorReference().GetFactionRank(PetFramework_PetFollowingFaction))

		;If the pet was waiting for the player, clear the actor value (no need to check just run it)
		WaitForPlayer(False)

		;Set the rank to 1, which will enable dialogue commands, etc. (used by shared pet framework)
		PetRefAlias.GetActorReference().SetFactionRank(PetFramework_PetFollowingFaction, 1)
	
		debug.trace("Pet Framework: " + PetRefAlias.GetActorReference() + " PetFollowingFactionRank: " + PetRefAlias.GetActorReference().GetFactionRank(PetFramework_PetFollowingFaction))
	
		;Some animals have very long idles they get 'stuck' which feels like recruiting them didn't do anything
		If(snapIntoInteraction)
			PetRefAlias.GetReference().Disable()
			PetRefAlias.GetReference().Enable()
		EndIf
	
		PetFramework_ParentQuest.IncrementPetCount()		
		debug.trace("Pet Count: " + PetFramework_ParentQuest.GetCurrentPetCount())
		
		;Re-evaluate the package stack based on our new conditions
		PetRefAlias.GetActorReference().EvaluatePackage()

	EndIf

EndFunction

Function WaitForPlayer(Bool doWait = True)
{True/False: Pet will wait for the player or continue to follow.}

	DEBUG.TRACE("WaitForPlayer called with value: " + doWait)

	If(doWait == True)
		debug.trace("PetFramework Setting pet to wait for player")
		PetRefAlias.GetActorReference().SetAV("WaitingForPlayer", 1)
	Else
		debug.trace("PetFramework Setting pet to stop waiting for player")
		PetRefAlias.GetActorReference().SetAV("WaitingForPlayer", 0)
	EndIf

	;Re-evaluate the package stack based on our new conditions
	PetRefAlias.GetActorReference().EvaluatePackage()
	
EndFunction

Function SetHomeToCurrentLocation()
{Makes the "Home" of the pet right where they stand, so they will dismiss and sandbox the area (house mods, camping, etc.)}

	;Move the xmarker to where the pet is standing
	PetDynamicHomeMarker.GetReference().MoveTo(PetRefAlias.GetReference())
	PetDynamicHomeMarker.GetReference().SetAngle(0,0,0)

	;Set the home idle marker to the dynamic marker
	SetNewHome(PetDynamicHomeMarker, True)

EndFunction

Function SetNewHome(ReferenceAlias newLocation, Bool dismiss = True, Bool doWarp = False)

	debug.trace("Set New Home called from actor proxy")

	;Clear the waiting flag
	WaitForPlayer(False)

	;Turn on the temporary package while we change the ref alias data for their idle package - trick from Hearthfire adoption
	MovingTogglePackageOn = True ;"Hold"AI package looks at this quest var
	PetRefAlias.GetActorReference().EvaluatePackage()

	;Set the ref alias to be the pet's new home
	PetHomeMarker.ForceRefTo(newLocation.GetReference())
	
	;Remove them from current following faction so they actually go to their new home
	If(dismiss)
		PetRefAlias.GetActorReference().SetFactionRank(PetFramework_PetFollowingFaction, 0)
	EndIf

	Utility.Wait(0.1)
	
	MovingTogglePackageOn = False
	PetRefAlias.GetActorReference().EvaluatePackage()
	
	If(doWarp)
		Utility.Wait(0.01)
		PetRefAlias.GetReference().MoveTo(newLocation.GetReference(), 0,0,0,False)
	EndIf

	PetFramework_PetDismissMessage.Show()
	PetFramework_ParentQuest.DecrementPetCount()
	
	debug.trace("Pet Count: " + PetFramework_ParentQuest.GetCurrentPetCount())
	
EndFunction



