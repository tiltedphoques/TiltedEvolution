from idaapi import *
from idc import *
import ctypes


info = idaapi.get_inf_structure()

addr_size = 4
if info.is_64bit():
    addr_size = 8

first_seg = FirstSeg()
last_seg = FirstSeg()
for seg in Segments():
    if seg > last_seg:
        last_seg = seg
    if seg < first_seg:
        first_seg = seg

def string_to_hex(str):
    return " ".join([ch.encode("hex") for ch in str])
        
def get_pointer(ea):
    if addr_size == 4:
        return Dword(ea)
    else:
        return Qword(ea)

def in_image(ea):
    return ea >= first_seg and ea <= SegEnd(last_seg)

def get_class_name(name_addr):
    s = Demangle('??_7' + GetString(name_addr + 4) + '6B@', 8)
    if s != None:
        return s[0:len(s)-11]
    else:
        return GetString(name_addr)

def GetVtable(rcol):
    for xref in XrefsTo(rcol):
        vtable = xref.frm + addr_size
        return vtable
        break

def GetVtableForClass(name):
    start = first_seg
    while True:
        f = FindBinary(start, SEARCH_DOWN, "2E 3F 41 56") # .?AV
        start = f + addr_size
        if f == BADADDR:
            return 0
        rtd = f - addr_size * 2
        className = get_class_name(f)
       
        if name == className:
            locator = (rtd - 0x140000000)
            str = ("%X" % locator)
            start = first_seg
            while True:
                f = FindBinary(start, SEARCH_DOWN, str)
                start = f + addr_size
                if f == BADADDR:
                    return 0
                
                for xref in XrefsTo(f - 0xC):
                    return xref.frm + addr_size
                for xref in XrefsTo(f):
                    return xref.frm + addr_size
        
def GetAbsoluteFromRegSet(ea):
    offset = ctypes.c_long(Dword(ea + 3)).value
    addr = ea + offset + 7
    if in_image(addr):
        return addr
        
def GetAbsoluteFromRel(ea):
    offset = ctypes.c_long(Dword(ea + 1)).value
    addr = ea + offset + 5
    if in_image(addr):
        if GetMnem(addr) == "jmp" or GetMnem(addr) == "call":
            return GetAbsoluteFromRel(addr)
        return addr
        
def GetCallableFunc(ea):
    for xref in XrefsTo(ea):
        if GetMnem(xref.frm) == "jmp":
            return GetCallableFunc(xref.frm)
    return ea   

def GetPapyrusFunctionClass(name, skip = 0):
    start = first_seg
        
    while True:
        f = FindBinary(start, SEARCH_DOWN, string_to_hex(name) + " 00")
        start = f + 4
        if f == BADADDR:
            break
        for xref in XrefsTo(f):
            if skip > 0:
                skip -= 1
                continue

            if SegName(xref.frm) == ".text":
                return GetString(GetAbsoluteFromRegSet(xref.frm - 0x7))
    return ""
        
def GetFunctionForObscript(name):
    start = first_seg
    while True:
        f = FindBinary(start, SEARCH_DOWN, string_to_hex(name) + " 00")
        start = f + 4
        if f == BADADDR:
            break
        for xref in XrefsTo(f):
            if SegName(xref.frm) == ".data":
                addr = Qword(xref.frm + 0x30)
                if in_image(addr):
                    return addr
    return 0
    
def GetFunctionForPapyrus(name, skip = 0):
    start = first_seg

    while True:
        f = FindBinary(start, SEARCH_DOWN, string_to_hex(name) + " 00")
        start = f + 4
        if f == BADADDR:
            break
        for xref in XrefsTo(f):
            if skip > 0:
                skip -= 1
                continue

            if SegName(xref.frm) == ".text":
                leaR9 = xref.frm - 0xE
                if GetMnem(leaR9) == "lea":
                    return GetAbsoluteFromRegSet(leaR9)
                leaRax = xref.frm + 0x12
                for i in xrange(0x9):
                    if GetMnem(leaRax + i) == "lea" and GetOpnd(leaRax + i, 0) == "rax":
                        return GetAbsoluteFromRegSet(leaRax + i)
                leaRax = xref.frm + 0x1B
                for i in xrange(0x9):
                    if GetMnem(leaRax + i) == "lea" and GetOpnd(leaRax + i, 0) == "rax":
                        return GetAbsoluteFromRegSet(leaRax + i)
    return 0

def GenerateDeclarationForPapyrus(name, count = 1):
    nativeClass = ""

    for i in range(0, count):
        papyrusFunc = GetFunctionForPapyrus(name, i)

        if count > 1:
            nativeClass = GetPapyrusFunctionClass(name, i) + "_"

        MakeNameEx(papyrusFunc, nativeClass + name, idc.SN_NOWARN)

        print ("RelocPtr<_" + nativeClass + name + "_Native> " + nativeClass + name + "_Native(0x%X - 0x140000000);" % papyrusFunc)
    
def GenerateDeclarationForObscript(name):
    papyrusFunc = GetFunctionForObscript(name)
    MakeNameEx(papyrusFunc, name, idc.SN_NOWARN)
    print ("RelocPtr<_" + name + "_Native> " + name + "_Native(0x%X - 0x140000000);" % GetFunctionForObscript(name))
    
def FindPerformAction():
    paVtbl = 0
    mediatorAddr = 0
    entry = GetFunctionForObscript("PerformAction")
    while GetMnem(entry) == "jmp":
        entry = GetAbsoluteFromRel(entry)
  
    actionInputCtor = GetAbsoluteFromRel(entry + 0x79) 
    actionInputRelease = GetAbsoluteFromRel(entry + 0x150)
        
    f = FindBinary(entry, SEARCH_DOWN, "48 8D 05")
    if f != BADADDR:
        paVtbl = GetAbsoluteFromRegSet(f)
        callAddr = f + 0xF
        if GetMnem(callAddr) == "call":
            TESActionPerform = GetAbsoluteFromRel(callAddr)

            MakeNameEx(TESActionPerform, "TESActionData::PerformAction", idc.SN_NOWARN)
            actorMediatorCs = TESActionPerform + 3
            if Byte(actorMediatorCs) == 0x48 and Byte(actorMediatorCs + 1) == 0x8B:
                mediatorAddr = GetAbsoluteFromRegSet(actorMediatorCs)
                entry = GetAbsoluteFromRel(TESActionPerform + 0xA)
                    
                print ("RelocPtr<void> g_actorMediator(0x%X - 0x140000000);" % mediatorAddr)
                print ("")
                print ("RelocPtr<_PerformAction_Native> PerformAction_Native(0x%X - 0x140000000);" % entry)
                MakeNameEx(entry, "ActorMediator::PerformAction", idc.SN_NOWARN)
                
    return paVtbl, actionInputCtor, actionInputRelease

def FindEventDispatcher():
    f = FindBinary(first_seg, SEARCH_DOWN, string_to_hex("Update budget: %fms"))
    callRegisterSinks = 0
    for xref in XrefsTo(f):
        callRegisterSinks = xref.frm - 0x62
        break
    
    if GetMnem(callRegisterSinks) != "call":
        print ("CALL REGISTER SINK ERROR 0x%x" % callRegisterSinks)
        return
        
    registerSinks = GetAbsoluteFromRel(callRegisterSinks)
    instructions = list(FuncItems(registerSinks))
    current = 0
    for instr in instructions:
        if GetMnem(instr) == "call":
            if current == 0:
                print ("RelocPtr<TGetDispatcherSingleton> kSingleton(0x%x - 0x140000000);" % GetOperandValue(instr, 0))
                current = 1
            else:
                print ("DEFINE_MEMBER_FN(AddEventSink_Internal, void, 0x%X - 0x140000000, SinkT * eventSink);" % GetOperandValue(instr, 0))
                break;
    
def VMFunctions():
    f = FindBinary(first_seg, SEARCH_DOWN, string_to_hex("Hitched from shader compile"))
    mainLoop = 0
    
    for xref in XrefsTo(f):
        mainLoop = GetFunctionAttr(xref.frm, FUNCATTR_START);
        print ("RelocPtr<TMainLoop> kMainLoop(0x%X - 0x140000000);" % mainLoop)
        break
    
    for xref in XrefsTo(GetCallableFunc(mainLoop)):
        if SegName(xref.frm) == ".text":
            addr = GetFunctionAttr(xref.frm, FUNCATTR_START)
            runnerAddr = GetCallableFunc(addr)
            for xrefR in XrefsTo(runnerAddr):
                if SegName(xrefR.frm) == ".text":
                    print "RelocPtr<TSomeDestructor> kSomeDestructor(0x%X - 0x140000000);" % GetAbsoluteFromRel(xrefR.frm + 5)

    f = FindBinary(first_seg, SEARCH_DOWN, string_to_hex("OnUpdate"))
    
    for xref in XrefsTo(f):
        addr = GetFunctionAttr(xref.frm, FUNCATTR_START);
        print ("RelocPtr<TVMUpdate> kVMUpdate(0x%X - 0x140000000);" % addr)
        break
    
def FaceTints():
    f = FindBinary(first_seg, SEARCH_DOWN, string_to_hex("BSLightingShaderProperty"))
  
    for xref in XrefsTo(f):
        if GetMnem(xref.frm) == "lea" and GetOpnd(xref.frm, 0) == "rdx":
            print ("RelocPtr<NiRTTI> kMaskedShaderRtti(0x%X - 0x140000000);" % GetOperandValue(xref.frm + 7, 1))

    f = FindBinary(first_seg, SEARCH_DOWN, string_to_hex("Player face tint"))  
    
    for xref in XrefsTo(f):
        funcStart = GetFunctionAttr(xref.frm, FUNCATTR_START)
    
        renderFromTintsCall = xref.frm - 9
        if GetMnem(renderFromTintsCall) == "call":
            print ("RelocPtr<TCreateTints> kCreateTints(0x%X - 0x140000000);" % GetOperandValue(renderFromTintsCall, 0))
        createTextureCall = xref.frm + 0x15
        if GetMnem(createTextureCall) == "call":
            print ("RelocPtr<TCreateTexture> kCreateTexture(0x%X - 0x140000000);" % GetOperandValue(createTextureCall, 0))
            
        createBufferSetup = FindBinary(xref.frm, SEARCH_DOWN, "BA 0F 00 00 00 48 8D 0D")
        if createBufferSetup != BADADDR:
            print ("RelocPtr<void> kSourceAddress(0x%X - 0x140000000);" % GetOperandValue(createBufferSetup + 5, 1))
            print ("RelocPtr<TDumpBuffer> kDumpBuffer(0x%X - 0x140000000);" % GetOperandValue(createBufferSetup + 0xC, 0))
            
        getObjectByNameSetup = FindBinary(funcStart, SEARCH_DOWN, "41 B0 01 48 8B C8 E8")
        if getObjectByNameSetup != BADADDR:
            print ("RelocPtr<TGetObjectByName> kGetObjectByName(0x%X - 0x140000000);" % GetOperandValue(getObjectByNameSetup + 6, 0))
        break  
       
            

def Texture():
    for xref in XrefsTo(GetVtableForClass("TESTexture")):
        print("DEFINE_MEMBER_FN(Construct, void, 0x%X - 0x140000000);" % GetFunctionAttr(xref.frm, FUNCATTR_START))
        break
        
        
# trick to disable stuff when testing :p
if True:
    Texture()
    print ""

    FaceTints()
    print ""

    FindEventDispatcher()
    print ""

    VMFunctions()
    print ("const RelocPtr<_LookupFormByID> LookupFormByID(0x%X - 0x140000000);" % GetAbsoluteFromRel(GetFunctionForPapyrus("GetForm") + 3))

    actionDataVtbl, actionInputCtor, actionInputRelease = FindPerformAction()

    print ("RelocPtr<_ShowMessageBox_Native> ShowMessageBox_Native(0x%X - 0x140000000);" % GetFunctionForPapyrus("MessageBox"))
    print ("RelocPtr<_Spell_Cast_Native> Spell_Cast_Native(0x%X - 0x140000000);" % GetFunctionForPapyrus("Cast", 2))
    GenerateDeclarationForObscript("IsMoving")
    GenerateDeclarationForObscript("IsPlayerMovingIntoNewSpace")
    GenerateDeclarationForObscript("MarkForDelete")
    GenerateDeclarationForPapyrus("Activate")
    GenerateDeclarationForPapyrus("AddItem")
    GenerateDeclarationForPapyrus("AddToMap")
    GenerateDeclarationForPapyrus("AllowPCDialogue")
    GenerateDeclarationForPapyrus("ClearLookAt")
    GenerateDeclarationForPapyrus("DamageActorValue")
    GenerateDeclarationForPapyrus("Delete")
    GenerateDeclarationForPapyrus("Disable")
    GenerateDeclarationForPapyrus("DisablePlayerControls")
    GenerateDeclarationForPapyrus("DropObject")
    GenerateDeclarationForPapyrus("Enable")
    GenerateDeclarationForPapyrus("EnablePlayerControls")
    GenerateDeclarationForPapyrus("EquipItem")
    GenerateDeclarationForPapyrus("EquipShout")
    GenerateDeclarationForPapyrus("EquipSpell")
    GenerateDeclarationForPapyrus("EvaluatePackage")
    GenerateDeclarationForPapyrus("FindWeather")
    GenerateDeclarationForPapyrus("ForceActive")
    GenerateDeclarationForPapyrus("ForceThirdPerson")
    GenerateDeclarationForPapyrus("GetBaseObject", 2)
    GenerateDeclarationForPapyrus("GetCombatTarget")
    GenerateDeclarationForPapyrus("GetCombatState")
    GenerateDeclarationForPapyrus("GetCrimeFaction")
    GenerateDeclarationForPapyrus("GetCrimeGold")
    GenerateDeclarationForPapyrus("GetCurrentWeather")
    GenerateDeclarationForPapyrus("GetEquippedShout")
    GenerateDeclarationForPapyrus("GetEquippedSpell")
    GenerateDeclarationForPapyrus("GetEquippedWeapon")
    GenerateDeclarationForPapyrus("GetFactionRank")
    GenerateDeclarationForPapyrus("GetLevel")
    GenerateDeclarationForPapyrus("GetOpenState")
    GenerateDeclarationForPapyrus("GetOutgoingWeather")
    GenerateDeclarationForPapyrus("GetPlayersLastRiddenHorse")
    GenerateDeclarationForPapyrus("GetRelationshipRank")
    GenerateDeclarationForPapyrus("Is3DLoaded")
    GenerateDeclarationForPapyrus("IsActivationBlocked")
    GenerateDeclarationForPapyrus("IsAlarmed")
    GenerateDeclarationForPapyrus("IsAlerted")
    GenerateDeclarationForPapyrus("IsArrestingTarget")
    GenerateDeclarationForPapyrus("IsCommandedActor")
    GenerateDeclarationForPapyrus("IsDead")
    GenerateDeclarationForPapyrus("IsDetectedBy")
    GenerateDeclarationForPapyrus("IsDisabled")
    GenerateDeclarationForPapyrus("IsEquipped")
    GenerateDeclarationForPapyrus("IsEssential", 2)
    GenerateDeclarationForPapyrus("IsGuard")
    GenerateDeclarationForPapyrus("IsHostileToActor")
    GenerateDeclarationForPapyrus("IsInDialogueWithPlayer")
    GenerateDeclarationForPapyrus("IsInvulnerable")
    GenerateDeclarationForPapyrus("IsLocked")
    GenerateDeclarationForPapyrus("IsProtected")
    GenerateDeclarationForPapyrus("IsSneaking")
    GenerateDeclarationForPapyrus("IsSprinting")
    GenerateDeclarationForPapyrus("IsUnique")
    GenerateDeclarationForPapyrus("InterruptCast")
    GenerateDeclarationForPapyrus("Kill")
    GenerateDeclarationForPapyrus("Lock")
    GenerateDeclarationForPapyrus("ModFactionRank")
    GenerateDeclarationForPapyrus("MoveTo")
    GenerateDeclarationForPapyrus("PathToReference")
    GenerateDeclarationForPapyrus("PlaceActorAtMe")
    GenerateDeclarationForPapyrus("PlaceAtMe")
    GenerateDeclarationForPapyrus("PlaySyncedAnimationSS")
    GenerateDeclarationForPapyrus("PushActorAway")
    GenerateDeclarationForPapyrus("ReleaseOverride")
    GenerateDeclarationForPapyrus("RemoveAllItems")
    GenerateDeclarationForPapyrus("RemoveFromFaction")
    GenerateDeclarationForPapyrus("RemoveItem")
    GenerateDeclarationForPapyrus("RequestAutoSave")
    GenerateDeclarationForPapyrus("RequestSave")
    GenerateDeclarationForPapyrus("RestoreActorValue")
    GenerateDeclarationForPapyrus("Resurrect")
    GenerateDeclarationForPapyrus("SendAnimationEvent")
    GenerateDeclarationForPapyrus("SendStealAlarm")
    GenerateDeclarationForPapyrus("SetActive", 2)
    GenerateDeclarationForPapyrus("SetAlert")
    GenerateDeclarationForPapyrus("SetAttackActorOnSight")
    GenerateDeclarationForPapyrus("SetBeastForm")
    GenerateDeclarationForPapyrus("SetCrimeFaction")
    GenerateDeclarationForPapyrus("SetCrimeGold")
    GenerateDeclarationForPapyrus("SetCurrentStageID")
    GenerateDeclarationForPapyrus("SetDontMove")
    GenerateDeclarationForPapyrus("SetEnemy")
    GenerateDeclarationForPapyrus("SetEssential")
    GenerateDeclarationForPapyrus("SetFactionRank")
    GenerateDeclarationForPapyrus("SetHeadTracking")
    GenerateDeclarationForPapyrus("SetInChargen")
    GenerateDeclarationForPapyrus("SetNotShowOnStealthMeter")
    GenerateDeclarationForPapyrus("SetOpen")
    GenerateDeclarationForPapyrus("SetPosition")
    GenerateDeclarationForPapyrus("SetRace")
    GenerateDeclarationForPapyrus("SetRelationshipRank")
    GenerateDeclarationForPapyrus("SetScale")
    GenerateDeclarationForPapyrus("SplineTranslateTo")
    GenerateDeclarationForPapyrus("StartCombat")
    GenerateDeclarationForPapyrus("StopCombat") 
    GenerateDeclarationForPapyrus("StopCombatAlarm")
    GenerateDeclarationForPapyrus("StopTranslation")
    GenerateDeclarationForPapyrus("TranslateTo")
    GenerateDeclarationForPapyrus("UnequipAll")
    GenerateDeclarationForPapyrus("UnequipItem")
    GenerateDeclarationForPapyrus("UnequipShout")
    GenerateDeclarationForPapyrus("UnequipSpell")
    GenerateDeclarationForPapyrus("Wait")
    
    print ("RelocPtr<void> vtbl(0x%X - 0x140000000);" % actionDataVtbl)
    print ("")
    print ("DEFINE_MEMBER_FN(ctor, ActionInput*, 0x%X - 0x140000000, UInt32 unkVar00, Actor* apActor, BGSAction* apAction, TESForm* apTarget);" % actionInputCtor)
    print ("DEFINE_MEMBER_FN(Release, void, 0x%X - 0x140000000);" % actionInputRelease)
