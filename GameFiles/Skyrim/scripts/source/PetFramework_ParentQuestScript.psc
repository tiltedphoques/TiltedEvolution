Scriptname PetFramework_ParentQuestScript extends Quest
{Get/Set functions for pet count and limit checks. All other shared controls are in PetFramework_PetScript.}
;rvogel 9/2017

GlobalVariable Property PetFramework_MaxPets Auto ;Max number of pets who can follow at once, default is 2

;RefAliases to fill from DLC at runtime

ReferenceAlias Property DLC2SeverinManorEnableMarker Auto
ReferenceAlias Property HomeMarkerDLC2SeverinManor Auto

ReferenceAlias Property BYOH01DoorLakeviewManor Auto
ReferenceAlias Property BYOH02DoorWindstadManor Auto
ReferenceAlias Property BYOH03DoorHeljarchenHall Auto

ReferenceAlias Property HomeMarkerLakeviewManor Auto
ReferenceAlias Property HomeMarkerWindstadManor Auto
ReferenceAlias Property HomeMarkerHeljarchenHall Auto

Faction Property PetFramework_PetFaction Auto
Faction Property PlayerFaction Auto

Faction Property CWSonsFaction Auto
Faction Property CWImperialFaction Auto

Int Property CurrentPetCount = 0 Auto Hidden ;Current count of pets, this is manipulated by pet ESPs/ESLs using the functions below

Event OnInit()
	debug.trace("Setting pet and player relationship to ally")
	PetFramework_PetFaction.SetAlly(PlayerFaction)
	
	;Prevent CW actors from becoming hostile to pets
	PetFramework_PetFaction.SetAlly(CWSonsFaction)
	PetFramework_PetFaction.SetAlly(CWImperialFaction)
EndEvent

Int Function GetCurrentPetCount()
{Called by 'child' pet ESPs/ESLs to get current count and limit}
	Return CurrentPetCount
EndFunction

Int Function GetMaxPets()
{Called by 'child' pet ESPs/ESLs to get max pets}
	Return PetFramework_MaxPets.GetValue() as Int
EndFunction

Function IncrementPetCount()
{Called by 'child' pet ESPs/ESLs to update active pet count}
	CurrentPetCount += 1
EndFunction

Function DecrementPetCount()
{Called by 'child' pet ESPs/ESLs to update active pet count}
	If(CurrentPetCount > 0)
		CurrentPetCount -= 1
	EndIf
EndFunction

Bool Function HasMaxPets()
{Called to check if the player has the maximum pets allowed}

	If(GetCurrentPetCount() == GetMaxPets())
		Return True
	Else
		Return False
	EndIf

EndFunction


Function FillRefAliasesFromDLC()
{Called from first stage of quest to fill aliases from DLC that are unreachable by Update.esm}
	
	;Dragonborn Refs (Marker and Chest used to check ownership)
	ObjectReference DLC2SeverinManorEnableRef = (Game.GetFormFromFile(0x040396D0, "dragonborn.esm") as ObjectReference)
	ObjectReference DLC2SeverinManorMarkerRef = (Game.GetFormFromFile(0x0403BD35, "dragonborn.esm") as ObjectReference)
	
	;Hearthfire Refs (Doors)
	ObjectReference DoorLakeviewManorRef = (Game.GetFormFromFile(0x03003221, "hearthfires.esm") as ObjectReference)
	ObjectReference DoorWindstadManorRef = (Game.GetFormFromFile(0x0300B852, "hearthfires.esm") as ObjectReference)
	ObjectReference DoorHeljarchenHallRef = (Game.GetFormFromFile(0x03010DDF, "hearthfires.esm") as ObjectReference)
	
	;Hearthfire Refs (Markers)
	ObjectReference MarkerLakeviewManorRef = (Game.GetFormFromFile(0x0300309B, "hearthfires.esm") as ObjectReference)
	ObjectReference MarkerWindstadManorRef = (Game.GetFormFromFile(0x0301205C, "hearthfires.esm") as ObjectReference)
	ObjectReference MarkerHeljarchenHallRef = (Game.GetFormFromFile(0x03016E05, "hearthfires.esm") as ObjectReference)

	;Fill the refs
	DLC2SeverinManorEnableMarker.ForceRefTo(DLC2SeverinManorEnableRef)
	HomeMarkerDLC2SeverinManor.ForceRefTo(DLC2SeverinManorMarkerRef)
	
	BYOH01DoorLakeviewManor.ForceRefTo(DoorLakeviewManorRef)
	BYOH02DoorWindstadManor.ForceRefTo(DoorWindstadManorRef)
	BYOH03DoorHeljarchenHall.ForceRefTo(DoorHeljarchenHallRef)
	
	HomeMarkerLakeviewManor.ForceRefTo(MarkerLakeviewManorRef)
	HomeMarkerWindstadManor.ForceRefTo(MarkerWindstadManorRef)
	HomeMarkerHeljarchenHall.ForceRefTo(MarkerHeljarchenHallRef)
	
EndFunction
