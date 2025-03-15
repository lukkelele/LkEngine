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
from pathlib import Path

verbose = False
clean = True

IsPlatformActionRunner = (os.environ.get("CI") == "true")
if IsPlatformActionRunner:
    verbose = True

import ScriptUtils as Utils

from ScriptUtils import ScriptLogger
Logger = ScriptLogger("LkEngine")

# Paths to Assimp, build and output directories.
if Path.is_dir(Path("../../LkEngine/External")):
    AssimpDir = os.path.join("..", "..", "LkEngine", "External", "Assimp", "assimp")
    BuildDir = os.path.join(AssimpDir, "build")
    OutputDir = os.path.join("..", "..", "LkEngine", "External", "Libraries")
elif Path.is_dir(Path("../LkEngine/External")):
    AssimpDir = os.path.join("..", "LkEngine", "External", "Assimp", "assimp")
    BuildDir = os.path.join(AssimpDir, "build")
    OutputDir = os.path.join("..", "LkEngine", "External", "Libraries")
else:
    print("Failed find assimp directory (LkEngine/External)");
    exit(1)

# Ensure output directory exists.
if verbose: print(f"[Build-Assimp] Output dir: {OutputDir}", flush=True)
os.makedirs(OutputDir, exist_ok=True)

SystemPlatform = platform.system()
IsPlatformWindows = SystemPlatform == "Windows"

Architecture = "x64" # TODO: Automatic assignment?
try:
    Architecture = "x64" if platform.architecture()[0] == "64bit" else "x86"
except Exception as e:
    print(f"[Build-Assimp] Error occured when trying to determine system architecture, error: {e}")
    exit(1)

if verbose: print(f"[Build-Assimp] Architecture: {Architecture}")
if Architecture != "x64":
    print(f"[Build-Assimp] Warning: System architecture is not 64-bit")

LibraryExtension = ".lib" if IsPlatformWindows else ".a"
if SystemPlatform == "Windows":
    LibraryFileDebug = "assimp-vc143-mtd" + LibraryExtension
    LibraryFileRelease = "assimp-vc143-mt" + LibraryExtension
elif SystemPlatform == "Linux":
    LibraryFileDebug = "libassimp" + LibraryExtension
    LibraryFileRelease = "libassimp" + LibraryExtension
else:
    print(f"[Build-Assimp] Unsupported platform")
    exit(1)

if verbose: print(f"[Build-Assimp] Build Dir: {Path(BuildDir).resolve()}", flush=True)

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
    "-DASSIMP_BUILD_OBJ_IMPORTER=ON", 
    "-DASSIMP_BUILD_OBJ_EXPORTER=ON",
]

def _RunCMakeConfiguration():
    """Run CMake configure command."""
    # TODO: Automatic assignment of architecture.
    result = None
    if verbose: print(f"[Build-Assimp] Current Directory: {Path(__file__).parent.resolve()}", flush=True)
    if verbose: print(f"[Build-Assimp] Current Working Directory: {Path().resolve()}", flush=True)
    if IsPlatformWindows:
        #result = subprocess.check_call(["cmake", "..", f"-A {Architecture}"] + CMakeFlags, cwd=BuildDir)
        result = subprocess.check_call(["cmake", "..", "-A", f"{Architecture}"] + CMakeFlags, cwd=BuildDir)
    else:
        result = subprocess.check_call(["cmake", ".."] + CMakeFlags, cwd=BuildDir)
    return result

def _BuildAssimp():
    """Run CMake build command."""
    if verbose: print(f"[Build-Assimp] Running: 'cmake --build . --config {BuildConfig}'", flush=True)
    build_command = ["cmake", "--build", ".", "--config", BuildConfig]
    result = subprocess.check_call(build_command, cwd=BuildDir)
    return result

def _CopyGeneratedFiles():
    """Copy the generated files to the desired locations."""
    if SystemPlatform == "Windows":
        SourceFile = os.path.join(BuildDir, "lib", BuildConfig, LibraryFile)
        DestinationFile = os.path.join(OutputDir, LibraryFile)
    elif SystemPlatform == "Linux":
        SourceFile = os.path.join(BuildDir, "lib", LibraryFile)
        DestinationFile = os.path.join(OutputDir, LibraryFile)
    else:
        print(f"[Build-Assimp] Unsupported platform", flush=True)
        exit(1)

    if verbose: print(f"[Build-Assimp] Copying static library\n  Source: {Path(SourceFile).resolve()}\n  Destination: {Path(DestinationFile).resolve()}\n", flush=True)
    shutil.copy2(SourceFile, DestinationFile)

    # Copy config.h and revision.h headers to Assimp's include directory.
    AssimpConfigHeader = Path(AssimpDir, "include/assimp/config.h")
    AssimpRevisionHeader = Path(AssimpDir, "include/assimp/revision.h")

    ConfigHeader = os.path.join(BuildDir, "include", "assimp", "config.h")
    RevisionHeader = os.path.join(BuildDir, "include", "assimp", "revision.h")

    if verbose: print(f"[Build-Assimp] Copying config header:\n  Source: {Path(ConfigHeader).resolve()}\n  Destination: {Path(AssimpConfigHeader).resolve()}\n", flush=True)
    if verbose: print(f"[Build-Assimp] Copying revision header:\n  Source: {Path(RevisionHeader).resolve()}\n  Destination: {Path(AssimpRevisionHeader).resolve()}\n", flush=True)
    shutil.copy2(ConfigHeader, AssimpConfigHeader)
    shutil.copy2(RevisionHeader, AssimpRevisionHeader)

    # Copy generated ZLib library.
    if SystemPlatform == "Windows":
        ZLibFileName = "zlibstaticd" if BuildAsDebug else "zlibstatic"
        ZLibFile = os.path.join(BuildDir, "contrib", "zlib", "Debug" if BuildAsDebug else "Release", ZLibFileName + LibraryExtension)
    elif SystemPlatform == "Linux":
        ZLibFileName = "libzlibstatic" if BuildAsDebug else "libzlibstatic"
        ZLibFile = os.path.join(BuildDir, "contrib", "zlib", ZLibFileName + LibraryExtension)
    else:
        print(f"[Build-Assimp] Unsupported platform", flush=True)
        exit(1)

    if verbose: print(f"[Build-Assimp] Copying: {ZLibFileName}{LibraryExtension} -> {OutputDir}")
    shutil.copy2(ZLibFile, os.path.join(OutputDir, ZLibFileName + LibraryExtension))

def _CleanUpAfterBuild():
    """Clean up build files."""
    if os.path.exists(BuildDir):
        if verbose: print(f"[Build-Assimp] Cleaning up build files from '{Path(BuildDir).resolve()}'", flush=True)
        shutil.rmtree(BuildDir)
    else:
        print(f"[Build-Assimp] Failed to clean up build directory as the dir could not be found: {Path(BuildDir).resolve()}", flush=True)

def BuildAssimp():
    if os.path.exists(BuildDir):
        print(f"[Build-Assimp] Build directory already exists: {BuildDir}", flush=True)
    else: 
        print(f"[Build-Assimp] Creating build directory: {BuildDir}", flush=True)
    os.makedirs(BuildDir, exist_ok=True)
    try:
        CMakeConfigResult = _RunCMakeConfiguration()
        BuildResult = _BuildAssimp()
        _CopyGeneratedFiles()
        if clean:
            _CleanUpAfterBuild()
        else:
            if verbose: print(f"[Build-Assimp] Skipping clean after build ({BuildDir})")
        return BuildResult 

    except subprocess.CalledProcessError as e:
        Logger.error(f"[Assimp] Error occurred during the build process: {e}")
        return 1 # Return non-zero to indicate error.

if __name__ == "__main__":
    BuildAssimp()
