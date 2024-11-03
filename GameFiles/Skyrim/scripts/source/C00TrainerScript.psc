Scriptname C00TrainerScript extends ReferenceAlias  

int numHits = 0

Event OnMagicEffectApply(ObjectReference akCaster, MagicEffect akEffect)
; 	Debug.Trace("C00: Vilkas hit by magic.")
;	if (Game.GetPlayer() == akCaster)
;		GetOwningQuest().SetStage(100)
;	endif
EndEvent

Event OnEnterBleedout()
	BleedoutChecks()
EndEvent

Function BleedoutChecks()
	if (!GetOwningQuest().IsRunning())
		return
	endif
; 	Debug.Trace("C00: Vilkas reached bleedout.")
	int currStage = GetOwningQuest().GetStage()
	if (currStage != 100 && currStage != 110) ; don't let it through if he's trying to
											  ;  razz you about using magic, for instance
		GetOwningQuest().SetStage(125)
	endif
EndFunction
;	Change for SkyrimTogetherReborn 1.6.7+animation is to allow more mods by not magic checking the hit which can have combat or perk overhaul hidden effects
Event OnHit(ObjectReference akAggressor, Form akSource, Projectile akProjectile, bool abPowerAttack, bool abSneakAttack, bool abBashAttack, bool abHitBlocked)
; 	Debug.Trace("C00: Vilkas was hit.")
; 	Debug.Trace("C00: Vilkas hit datums -- " + akAggressor + " " + akSource + " " + akProjectile + " " + abPowerAttack + " " + abSneakAttack + " " + abBashAttack + " " + abHitBlocked)
;	if     (akSource as Spell)
; 		Debug.Trace("C00: Vilkas hit by spell; bailing out and handling it in the magic effect handler.")
;		return
;	elseif (akSource as Explosion)
; 		Debug.Trace("C00: Vilkas hit by explosion; bailing out and handling it in the other handlers.")
;		return
;	endif
;	if (Game.GetPlayer() == akAggressor)
;		if ((akSource as Spell) != None)
;			GetOwningQuest().SetStage(100)
;			return
;		endif
		numHits += 1
		if (numHits >= 3)
			GetOwningQuest().SetStage(150)
		endif
;	else
;		; someone else hit him, stop the quest and have him berate you
;		GetOwningQuest().SetStage(110)
;	endif
EndEvent

Function ResetHits()
	numHits = 0
EndFunction
