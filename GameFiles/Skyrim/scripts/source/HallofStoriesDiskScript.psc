scriptName HallofStoriesDiskScript extends ObjectReference
{
- This script is found on each rotating ring of the door used in the Hall of Stories
- User can use properties to set the ring's initial state and solve state
- The script first checks to see if the initial state is the same as the solve state
- When the ring is activated, it rotates to a new position and then checks to see if its solved
- Each ring should have its linkedRef point to the keyhole piece
}


import debug
import utility

bool ringSolved
bool doOnce

HallofStoriesKeyholeScript myLinkedRef

int property initialState auto
{
This is the position the pillar starts out in.
Starting at top and going Counter Clockwise
1 = Position 1
2 = Position 2
3 = Position 3
Position 1,2,3 refer to the havok animations
}

int property solveState auto
{
This is the position the pillar needs to be in to be considered solved.
Starting at top and going Counter Clockwise
1 = Position 1
2 = Position 2
3 = Position 3
Position 1,2,3 refer to the havok animations
}

Event onCellLoad()

	;teammates shouldn't use this ever`
	Self.SetNoFavorAllowed()

	myLinkedRef = GetLinkedRef() as HallofStoriesKeyholeScript
	
	;account for case where initialState and solveState are equal in the beginning
	if(!doOnce)
		if (initialState == solveState)
			myLinkedRef.numRingsSolved = myLinkedRef.numRingsSolved + 1
			ringSolved = true
		endif
		
		;now move the pillar into initialState
		;trace(self + " initialstate " + initialState)
		if (initialState == 1)
			;no animation
			gotoState("position01")
			;notification("initial state = 1")
		elseif (initialState == 2)
			playAnimation("StartState02")
			gotoState("position02")
			;notification("initial state = 2")
		elseif (initialState == 3)
			playAnimation("StartState03")
			gotoState("position03")
			;notification("initial state = 3")
		endif
		
		doOnce = true
	endif
		
endEvent

; Makes the disk rotate to the next state and checks whether it is 'solved' as a result	
FUNCTION RotateRingToState(int stateNumber, int animEventNumber)
		if (solveState == stateNumber)
			myLinkedRef.numRingsSolved = myLinkedRef.numRingsSolved + 1
			ringSolved = true
		elseif (ringSolved == True)
			if (myLinkedRef.puzzleSolved == true)
				myLinkedRef.puzzleSolved = false
			endif
			if myLinkedRef.numRingsSolved > 0
				myLinkedRef.numRingsSolved = myLinkedRef.numRingsSolved - 1
			endIf
			ringSolved = False
		endif
		playAnimationandWait("Trigger0" + animEventNumber, "Turned0" + animEventNumber)
endFUNCTION

	
STATE position01
	EVENT onActivate (objectReference triggerRef)
		Actor actorRef = triggerRef as Actor
		if ((SkyrimTogetherUtils.IsPlayer(actorRef)) && (myLinkedRef.GetState() != "busy"))
			gotoState ("busy")
			RotateRingToState(2, 1)
			gotoState("position02")
		endif
	endEVENT
endState


STATE position02
	EVENT onActivate (objectReference triggerRef)
		Actor actorRef = triggerRef as Actor
		if ((SkyrimTogetherUtils.IsPlayer(actorRef)) && (myLinkedRef.GetState() != "busy"))
			gotoState ("busy")
			RotateRingToState(3, 2)
			gotoState("position03")
		endif
	endEVENT
endState


STATE position03
	EVENT onActivate (objectReference triggerRef)
		Actor actorRef = triggerRef as Actor
		if ((SkyrimTogetherUtils.IsPlayer(actorRef)) && (myLinkedRef.GetState() != "busy"))
			gotoState ("busy")
			RotateRingToState(1, 3)
			gotoState("position01")
		endif
	endEVENT
endState


STATE busy
	; This is the state when I'm busy animating
		EVENT onActivate (objectReference triggerRef)
			;busy animating
		endEVENT
endSTATE

