import idc
import ida_typeinf
import ida_kernwin
import ida_nalt
import idaapi

def extract_function_id(func):
    comment = idc.get_cmt(func, True)
    return comment[5:]

def get_function_info(func):
    tinfo = ida_typeinf.tinfo_t()
    ida_nalt.get_tinfo(tinfo, func)
    function_details = idaapi.func_type_data_t()
    tinfo.get_func_details(function_details)

    return_type = tinfo.get_rettype().dstr()

    arguments = ""
    argument_names = ""
    for i in range(function_details.size()):
        if i != 0:
            arguments = arguments + ", "
            argument_names = argument_names + ", "
        
        argument_name = function_details[i].name
        if argument_name == "this":
            argument_name = "apThis"

        arguments = arguments + "{} {}".format(ida_typeinf.print_tinfo('', 0, 0, idc.PRTYPE_1LINE, function_details[i].type, '', ''), argument_name)
        argument_names = argument_names + argument_name

    return return_type, arguments, argument_names

def generate_hook(ea):
    func_id = extract_function_id(ea)
    func_name = idc.get_func_name(ea)
        
    demangled_name: str = idc.demangle_name(func_name, 8)
    if demangled_name != None:
        func_name = demangled_name
        
    if "::" in func_name:
        func_name = func_name.split("::")[-1]
        
    return_type, arguments, argument_names = get_function_info(ea)

    hook_signature = f"{return_type} __fastcall Hook{func_name}({arguments})"

    print(f"""
#if TP_SKYRIM64
{hook_signature};
using T{func_name} = decltype(Hook{func_name});
static T{func_name}* Real{func_name} = nullptr;

{hook_signature}
{{
    return TiltedPhoques::ThisCall(Real{func_name}, {argument_names});
}}

static TiltedPhoques::Initializer s_{func_name}HookInitializer(
    []()
    {{
        POINTER_SKYRIMSE(T{func_name}, s_{func_name}, {func_id});
        Real{func_name} = s_{func_name}.Get();
        TP_HOOK(&Real{func_name}, Hook{func_name});
    }});
#endif
""")

if __name__ == "__main__":
    generate_hook(ida_kernwin.get_screen_ea())