import os
from pathlib import Path

# run this script from Tools/Scripts/
rootdir = Path(__file__).parent.parent.parent.joinpath("Code").joinpath("client")

# collect all paths in the Client/ folder
paths = []

for subdir, dirs, files in os.walk(rootdir):
	for file in files:
		if ".h" in file or ".cpp" in file:
			paths.append(os.path.join(subdir, file))

with open('skyrim_addresses.txt', 'w') as output_file:
	for path in paths:
		with open(path, 'r') as source_file:
			for line in source_file:
				if "POINTER_SKYRIMSE" in line:
					if "#define POINTER_SKYRIMSE" in line:
						print("Define found, skipping line.")
						continue

					begin_pos = line.find("0x14")
					if begin_pos == -1:
						print(f"Something went wrong in file {file}, line {line}")
						continue

					end_pos = begin_pos + 11
					address = line[begin_pos:end_pos]
					output_file.write(address + "\n")

lines_seen = set()

with open('skyrim_addresses.txt', 'r+') as output_file:
	lines = output_file.readlines()
	output_file.seek(0)
	for line in lines:
		if line in lines_seen:
			print(f"duplicate! {line}")
		else:
			output_file.write(line)
			lines_seen.add(line)
	output_file.truncate()