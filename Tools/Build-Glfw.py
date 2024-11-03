#--------------------------------------------------------------------------
# [LkEngine] Build_Glfw.py
#
# INTERNAL USE ONLY
#
# Build GLFW and move the created library file to 'External/Libraries'.
#--------------------------------------------------------------------------
import os
import platform
import subprocess
import shutil

import ScriptUtils as Utils

from ScriptUtils import ScriptLogger
Logger = ScriptLogger("LkEngine")

# Paths to GLFW, build and output directories.
glfw_dir = os.path.join("..", "External", "glfw")
build_dir = os.path.join(glfw_dir, "build")
output_dir = os.path.join("..", "External", "Libraries")

# Ensure output directory exists.
os.makedirs(output_dir, exist_ok=True)

# CMake configuration flags.
# Build command: cmake .. -DBUILD_SHARED_LIBS=OFF -DGLFW_LIBRARY_TYPE=STATIC -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_TESTS=OFF
cmake_flags = [
    "-DBUILD_SHARED_LIBS=OFF",
    "-DGLFW_LIBRARY_TYPE=STATIC",
    "-DGLFW_BUILD_EXAMPLES=OFF",
    "-DGLFW_BUILD_DOCS=OFF",
    "-DGLFW_BUILD_TESTS=OFF"
]

# Platform-specific options.
PlatformWindows = platform.system() == "Windows"
library_extension = ".lib" if PlatformWindows else ".a"

def _RunCMakeConfiguration():
    """Run CMake configure command."""
    Logger.info("[Glfw] Running CMake configurations")
    subprocess.check_call(["cmake", ".."] + cmake_flags, cwd=build_dir)

def _BuildGlfw():
    """Run CMake build command."""
    build_command = ["cmake", "--build", ".", "--config", "Release"] if PlatformWindows else ["make", "-j"]
    result = subprocess.check_call(build_command, cwd=build_dir)
    return result

def _MoveGeneratedLibraryFile():
    """Copy the generated build file to the desired location."""
    Logger.info(f"Copying library file to: '{output_dir}'")
    src_file = os.path.join(build_dir, "src", "Release", "glfw3" + library_extension) if PlatformWindows else os.path.join(build_dir, "src", "libglfw3.a")
    dest_file = os.path.join(output_dir, "glfw3" + library_extension)
    shutil.copy2(src_file, dest_file)
    #Logger.info(f"Library file can be found in: {dest_file}")

def _CleanUpAfterBuild():
    """Clean up build files."""
    if os.path.exists(build_dir):
        shutil.rmtree(build_dir)
        #Logger.info(f"Cleaned up build directory: {build_dir}")

def BuildGlfw():
    os.makedirs(build_dir, exist_ok=True)
    try:
        _RunCMakeConfiguration()
        SetupResult = _BuildGlfw()
        _MoveGeneratedLibraryFile()
        _CleanUpAfterBuild()
        return SetupResult

    except subprocess.CalledProcessError as e:
        Logger.error(f"[Glfw] Error occurred during the build process: {e}")
        return False

if __name__ == "__main__":
    BuildGlfw()
