import glob
import re

def parse_call(line):
    block_count = 0
    end_idx = 0
    start_idx = len('RTTI_CAST(')

    for i in range(start_idx, len(line)):
        if line[i] == '(':
            block_count = block_count + 1
        elif line[i] == ')':
            block_count = block_count - 1
        elif line[i] == ',' and block_count == 0:
            end_idx = i
            break

    from_type = line.find(',', end_idx + 1)
    to_type = line.find(')', from_type + 1)

    inst = line[start_idx:end_idx]
    type_target = line[from_type + 2:to_type]
    rep = line[0:to_type+1]

    return ("Cast<" + type_target + ">(" + inst + ")", rep)

def handle_file(filename):
    if 'Fallout4' in filename or 'RTTI.h' in filename or 'RTTI.cpp' in filename:
        return

    with open(filename, 'r') as file :
        filedata = file.read()

    replace = {}
    pointers = [m.start() for m in re.finditer('RTTI_CAST', filedata)]
    if len(pointers) > 0:
        for p in pointers:
            start_loc = p
            end_loc = filedata.find('\n', start_loc)

            to, fr = parse_call(filedata[start_loc:end_loc])
            replace[fr] = to

    if len(replace) > 0:
        for fr in replace:
            filedata = filedata.replace(fr, replace[fr])

        with open(filename, 'w') as file:
            file.write(filedata)

# root_dir needs a trailing slash (i.e. /root/dir/)
for filename in glob.iglob('../../Code/**/*.cpp', recursive=True):
    handle_file(filename)

for filename in glob.iglob('../../Code/**/*.hpp', recursive=True):
    handle_file(filename)

for filename in glob.iglob('../../Code/**/*.h', recursive=True):
    handle_file(filename)
