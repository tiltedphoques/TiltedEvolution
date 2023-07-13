import glob
import re
import ida_funcs
import idc

def get_codebase_mapping():
    codebase_mapping = {}
    root_dir = "" # TiltedEvolution/Code/
    globbings = ["**/*.cpp", "**/*.h", "**/*.hpp"]
    for globbing in globbings:
        for filename in glob.iglob(root_dir + globbing, recursive=True):
            if "Utils.h" in filename or "EventDispatcher.h" in filename:
                continue
            
            with open(filename, 'r') as file:
                filedata = file.read()

            pointers = [m.start() for m in re.finditer('POINTER_SKYRIMSE', filedata)]

            for pointer in pointers:
                name_begin = filedata.find('(', pointer) + 1
                name_end = filedata.find(',', name_begin)
                name = filedata[name_begin:name_end]

                func_begin = filedata.find(' ', pointer) + 1
                id_begin = filedata.find(' ', func_begin) + 1
                id_end = filedata.find(')', id_begin)
                try:
                    id = int(filedata[id_begin:id_end])
                    codebase_mapping[id] = (name, filename.split('\\')[-1])
                except:
                    print(f"Failed on {name} in {filename}")

    return codebase_mapping

def get_id_address_mapping():
    id_address_mapping = {}
    filename = ""
    with open(filename, 'r') as file:
        for line in file:
            id_to_address = line.split('\t')
            id = int(id_to_address[0])
            address = int(id_to_address[1].rstrip('\n'), 0)
            id_address_mapping[id] = address
    return id_address_mapping

def print_idb_functions_to_codebase(codebase_mapping, id_address_mapping):
    unnamed_functions_count = 0
    non_function_count = 0

    for id, data in codebase_mapping.items():
        address = id_address_mapping[id]
        name, filename = data
        
        func = ida_funcs.get_func(address)
        if func == None:
            #print(f"No function found for id {id}, function {name}, filename {filename}")
            non_function_count = non_function_count + 1
            continue
        
        idb_name = ida_funcs.get_func_name(address)
        demangled_idb_name = idc.demangle_name(idb_name, 8)
        if demangled_idb_name != None:
            idb_name = demangled_idb_name
        
        if idb_name[0:4] == "sub_":
            #print(f"No user defined function for {name}")
            unnamed_functions_count = unnamed_functions_count + 1
            continue
        
        print(f"Codebase name: {name}, idb name: {idb_name}, filename: {filename}, id: {id}, address: {hex(address)}")
    
    print(f"\nUnnamed functions: {unnamed_functions_count}, non functions: {non_function_count}")


if __name__ == "__main__":
    codebase_mapping = get_codebase_mapping()
    id_address_mapping = get_id_address_mapping()
    print_idb_functions_to_codebase(codebase_mapping, id_address_mapping)

