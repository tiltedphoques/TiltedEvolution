from idaapi import *
from idc import *

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

def dump_hierarchy(rchd, rcol):
    rchd_numBaseClasses = Dword(rchd + 8)
    rchd_pBaseClassArray = get_pointer(rchd + 12)
    print "  Num base: %d" % rchd_numBaseClasses
    print "  Base ptr: %X" % rchd_pBaseClassArray
    for i in xrange(rchd_numBaseClasses):
        rbcd = get_pointer(rchd_pBaseClassArray + addr_size * i)
        rbcd_pTypeDescriptor = get_pointer(rbcd)
        print "TD: %X" % (rbcd_pTypeDescriptor)
        rbcd_pTypeDescriptor_name = get_class_name(rbcd_pTypeDescriptor + 8)
        print "  - base class: %s" % rbcd_pTypeDescriptor_name
    for xref in XrefsTo(rcol):
        vtable = xref.frm + addr_size
        print "  - vtable: 0x%X" % vtable
        break

def dump_rtti(rtd, name):
    if name.isalnum():
        print "AutoPtr<const void> RTTI_" + name + "(0x%X);" % (rtd - idaapi.get_imagebase())

print "==Source=="

ea = BeginEA()
for funcea in Functions(SegStart(ea), SegEnd(ea)):
    functionName = GetFunctionName(funcea)
    if functionName == "__RTDynamicCast":
        print "const AutoPtr<_Runtime_DynamicCast> Runtime_DynamicCast(0x%X);" % (funcea - idaapi.get_imagebase())
        print ""
        break

start = first_seg
while True:
    f = FindBinary(start, SEARCH_DOWN, "2E 3F 41 56") # .?AV
    start = f + addr_size
    if f == BADADDR:
        break
    rtd = f - addr_size * 2
    name = get_class_name(f)
    dump_rtti(rtd, name)
    #print "Found class: %s (rtd=0x%X)" % (name, rtd)
    for xref in XrefsTo(rtd):
        if SegName(xref.frm) == ".rdata":
            rchd = get_pointer(xref.frm + addr_size)
            rchd_classinformer = get_pointer(xref.frm + addr_size + 12)
            if in_image(rchd):
                rcol = xref.frm - 12
                #dump_hierarchy(rchd, rcol)
            elif in_image(rchd_classinformer):
                rcol = xref.frm
                #dump_hierarchy(rchd_classinformer, rcol)

print "==Header=="

start = first_seg
while True:
    f = FindBinary(start, SEARCH_DOWN, "2E 3F 41 56") # .?AV
    start = f + addr_size
    if f == BADADDR:
        break
    rtd = f - addr_size * 2
    name = get_class_name(f)
    if name.isalnum():
        print "extern template internal::RttiLocator<" + name +"> RTTI_" + name + ";"
