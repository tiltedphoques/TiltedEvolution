import os
from pathlib import Path

rootdir = Path(__file__).parent.parent.parent.joinpath("Code").joinpath("client")

paths = []

for subdir, dirs, files in os.walk(rootdir):
	"""
	print("======== SUBDIR ========")
	print(subdir)
	print("======== DIRS ========")
	print(dirs)
	print("======== FILES ========")
	"""
	for file in files:
		if ".h" in file or ".cpp" in file:
			paths.append(os.path.join(subdir, file))

for path in paths:
	print(path)

with open('new_addresses.txt') as new_addresses
	for address_pair in new_addresses:
		address_pair = address_pair.split(',')

		for path in paths:
			with open(path, 'r') as source_file:
				file_data = source_file.read()

		if address_pair[0] in file_data:
			file_data.replace(address_pair[0], address_pair[1])
			continue
