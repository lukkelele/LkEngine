#--------------------------------------------------------------------------
# [LkEngine] Build_Assimp.py
#
# INTERNAL USE ONLY
#
# Build Assimp and move the created static library to 'External/Libraries'.
#
# TODO: Build Debug/Release and move both static libs directly.
#--------------------------------------------------------------------------
import os
import platform
import subprocess
import shutil

import ScriptUtils as Utils

from ScriptUtils import ScriptLogger
Logger = ScriptLogger("LkEngine")

# Paths to Assimp, build and output directories.
AssimpDir = os.path.join("..", "..", "External", "assimp")
BuildDir = os.path.join(AssimpDir, "build")
OutputDir = os.path.join("..", "..", "External", "Libraries")

# Ensure output directory exists.
os.makedirs(OutputDir, exist_ok=True)

IsPlatformWindows = platform.system() == "Windows"
LibraryExtension = ".lib" if IsPlatformWindows else ".a"
LibraryFileRelease = "assimp-vc143-mt" + LibraryExtension
LibraryFileDebug = "assimp-vc143-mtd" + LibraryExtension

# TODO: Automatic selection of Debug/Release.
BuildAsDebug = True
BuildConfig = "debug" if BuildAsDebug else "release"
LibraryFile = LibraryFileDebug if BuildAsDebug else LibraryFileRelease

# CMake flags.
cmake_flags = [
    "-DBUILD_SHARED_LIBS=OFF",
    "-DASSIMP_BUILD_TESTS=OFF",
]

# Platform-specific options.
IsPlatformWindows = platform.system() == "Windows"

def _RunCMakeConfiguration():
    """Run CMake configure command."""
    result = subprocess.check_call(["cmake", ".."] + cmake_flags, cwd=BuildDir)
    return result

def _BuildAssimp():
    """Run CMake build command."""
    build_command = ["cmake", "--build", ".", "--config", BuildConfig] if IsPlatformWindows else ["make", "-j"]
    result = subprocess.check_call(build_command, cwd=BuildDir)
    return result

def _CopyGeneratedFiles():
    """Copy the generated files to the desired locations."""
    SourceFile = os.path.join(BuildDir, "lib", BuildConfig, LibraryFile)
    ConfigHeader = os.path.join(BuildDir, "include", "assimp", "config.h")
    RevisionHeader = os.path.join(BuildDir, "include", "assimp", "revision.h")
    DestinationFile = os.path.join(OutputDir, LibraryFile)

    shutil.copy2(SourceFile, DestinationFile)
    shutil.copy2(ConfigHeader, os.path.join(AssimpDir, "include", "assimp", "config.h"))
    shutil.copy2(RevisionHeader, os.path.join(AssimpDir, "include", "assimp", "revision.h"))

def _CleanUpAfterBuild():
    """Clean up build files."""
    if os.path.exists(BuildDir):
        shutil.rmtree(BuildDir)
        #print("CLEAN UP - REMOVE ME")

def BuildAssimp():
    os.makedirs(BuildDir, exist_ok=True)
    try:
        CMakeConfigResult = _RunCMakeConfiguration()
        BuildResult = _BuildAssimp()
        _CopyGeneratedFiles()
        _CleanUpAfterBuild()
        return BuildResult 

    except subprocess.CalledProcessError as e:
        Logger.error(f"[Assimp] Error occurred during the build process: {e}")
        return 1 # Return non-zero to indicate error.

if __name__ == "__main__":
    BuildAssimp()
