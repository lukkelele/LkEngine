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
from sys import exit

import ScriptUtils as Utils

from ScriptUtils import ScriptLogger
Logger = ScriptLogger("LkEngine")

# Paths to Assimp, build and output directories.
AssimpDir = os.path.join("..", "..", "External", "Assimp", "assimp")
BuildDir = os.path.join(AssimpDir, "build")
OutputDir = os.path.join("..", "..", "External", "Libraries")

# Ensure output directory exists.
os.makedirs(OutputDir, exist_ok=True)

IsPlatformWindows = platform.system() == "Windows"
if not IsPlatformWindows:
    print(f"[Build-Assimp] Platform is not Windows, undefined behaviour can be expected")

Architecture = "x64" # TODO: Automatic assignment
try:
    Architecture = "x64" if platform.architecture()[0] == "64bit" else "x86"
except Exception as e:
    print(f"[Build-Assimp] Error occured when trying to determine system architecture, error: {e}")
    exit(1)

if Architecture != "x64":
    print(f"[Build-Assimp] Warning: System architecture is not 64-bit")

LibraryExtension = ".lib" if IsPlatformWindows else ".a"
LibraryFileRelease = "assimp-vc143-mt" + LibraryExtension
LibraryFileDebug = "assimp-vc143-mtd" + LibraryExtension

# TODO: Automatic selection of Debug/Release.
BuildAsDebug = True
BuildConfig = "debug" if BuildAsDebug else "release"
LibraryFile = LibraryFileDebug if BuildAsDebug else LibraryFileRelease

# CMake flags.
CMakeFlags = [
    "-DBUILD_SHARED_LIBS=OFF",
    "-DASSIMP_BUILD_TESTS=OFF",
	"-DASSIMP_INSTALL=OFF",
    "-DUSE_STATIC_CRT=ON",
	"-DASSIMP_BUILD_ZLIB=ON",
    "-DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=OFF",
    "-DASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT=OFF",
    "-DASSIMP_BUILD_FBX_IMPORTER=ON", 
    "-DASSIMP_BUILD_FBX_EXPORTER=ON",
    "-DASSIMP_BUILD_GLTF_IMPORTER=ON",
    "-DASSIMP_BUILD_GLTF_EXPORTER=ON",
]

# Platform-specific options.
IsPlatformWindows = platform.system() == "Windows"

def _RunCMakeConfiguration():
    """Run CMake configure command."""
    # TODO: Automatic assignment of architecture.
    result = subprocess.check_call(["cmake", "..", f"-A {Architecture}"] + CMakeFlags, cwd=BuildDir)
    return result

def _BuildAssimp():
    """Run CMake build command."""
    build_command = ["cmake", "--build", ".", "--config", BuildConfig] if IsPlatformWindows else ["make", "-j"]
    result = subprocess.check_call(build_command, cwd=BuildDir)
    return result

def _CopyGeneratedFiles():
    """Copy the generated files to the desired locations."""
    SourceFile = os.path.join(BuildDir, "lib", BuildConfig, LibraryFile)
    DestinationFile = os.path.join(OutputDir, LibraryFile)
    shutil.copy2(SourceFile, DestinationFile)

    # Copy config.h and revision.h headers to Assimp's include directory.
    ConfigHeader = os.path.join(BuildDir, "include", "assimp", "config.h")
    RevisionHeader = os.path.join(BuildDir, "include", "assimp", "revision.h")
    shutil.copy2(ConfigHeader, os.path.join(AssimpDir, "include", "assimp", "config.h"))
    shutil.copy2(RevisionHeader, os.path.join(AssimpDir, "include", "assimp", "revision.h"))

    # Copy generated ZLib library.
    ZLibFileName = "zlibstaticd" if BuildAsDebug else "zlibstatic"
    ZLibFile = os.path.join(BuildDir, "contrib", "zlib", "Debug" if BuildAsDebug else "Release", ZLibFileName + LibraryExtension)
    shutil.copy2(ZLibFile, os.path.join(OutputDir, ZLibFileName + LibraryExtension))

def _CleanUpAfterBuild():
    """Clean up build files."""
    if os.path.exists(BuildDir):
        shutil.rmtree(BuildDir)

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
