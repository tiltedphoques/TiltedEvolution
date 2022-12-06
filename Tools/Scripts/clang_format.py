import os
import subprocess

def format_cpp_file(file_path):
  # Run clang-format on the file to format it
  subprocess.run(["clang-format", "-i", file_path, "-style=file:" + os.getcwd() + "/.clang-format", "-fallback-style=none"])

def recursive_directory_iteration(directory):
  # Iterate over all of the files in the directory
  for filename in os.listdir(directory):
    # Get the full path of the file
    file_path = os.path.join(directory, filename)
    
    # If the file is a directory, recursively iterate over it
    if os.path.isdir(file_path):
      recursive_directory_iteration(file_path)
    # If the file is a cpp file, format it
    elif file_path.endswith(".cpp") or file_path.endswith(".h"):
      format_cpp_file(file_path)

print("Formatting C++ files...")
print("Found clang format config at " + os.getcwd() + "/.clang-format")
# Start the recursive iteration from the current directory
recursive_directory_iteration(os.getcwd())