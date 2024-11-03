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
GlfwDir = os.path.join("..", "..", "External", "glfw")
BuildDir = os.path.join(GlfwDir, "build")
OutputDir = os.path.join("..", "..", "External", "Libraries")

# Ensure output directory exists.
os.makedirs(OutputDir, exist_ok=True)

# CMake configuration flags.
CMakeFlags = [
    "-DBUILD_SHARED_LIBS=OFF",
    "-DGLFW_LIBRARY_TYPE=STATIC",
    "-DGLFW_BUILD_EXAMPLES=OFF",
    "-DGLFW_BUILD_DOCS=OFF",
    "-DGLFW_BUILD_TESTS=OFF"
]

# Platform-specific options.
IsPlatformWindows = platform.system() == "Windows"
library_extension = ".lib" if IsPlatformWindows else ".a"

def _RunCMakeConfiguration():
    """Run CMake configure command."""
    subprocess.check_call(["cmake", ".."] + CMakeFlags, cwd=BuildDir)

def _BuildGlfw():
    """Run CMake build command."""
    build_command = ["cmake", "--build", ".", "--config", "Release"] if IsPlatformWindows else ["make", "-j"]
    result = subprocess.check_call(build_command, cwd=BuildDir)
    return result

def _MoveGeneratedLibraryFile():
    """Copy the generated library to the desired location."""
    #Logger.info(f"Copying library file to: '{OutputDir}'")
    src_file = os.path.join(BuildDir, "src", "Release", "glfw3" + library_extension) if IsPlatformWindows else os.path.join(BuildDir, "src", "libglfw3.a")
    dest_file = os.path.join(OutputDir, "glfw3" + library_extension)
    shutil.copy2(src_file, dest_file)

def _CleanUpAfterBuild():
    """Clean up build files."""
    if os.path.exists(BuildDir):
        shutil.rmtree(BuildDir)

def BuildGlfw():
    os.makedirs(BuildDir, exist_ok=True)
    try:
        _RunCMakeConfiguration()
        SetupResult = _BuildGlfw()
        _MoveGeneratedLibraryFile()
        _CleanUpAfterBuild()
        return SetupResult

    except subprocess.CalledProcessError as e:
        Logger.error(f"[Glfw] Error occurred during the build process: {e}")
        return 1 # Return non-zero to indicate error.

if __name__ == "__main__":
    BuildGlfw()
