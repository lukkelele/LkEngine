#--------------------------------------------------------------------------
# [LkEngine] Build_Assimp.py
#
# INTERNAL USE ONLY
#
# Build Assimp and move the created static library to 'External/Libraries'.
#--------------------------------------------------------------------------
import sys
import re
import os

file_dirname = os.path.basename(os.path.dirname(__file__))
if file_dirname == "LkEngine":
    sys.path.append(os.path.join(os.path.dirname(__file__), "Tools/Module"))
elif file_dirname == "Tools":
    sys.path.append(os.path.dirname(__file__))

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

if file_dirname == "LkEngine":
    import Tools.Module.ScriptUtils as Utils
    from Tools.Module.ScriptUtils import ScriptLogger
elif file_dirname == "Tools":
    import Module.ScriptUtils as Utils
    from Module.ScriptUtils import ScriptLogger
elif file_dirname == "Module":
    import ScriptUtils as Utils
    from ScriptUtils import ScriptLogger

Logger = ScriptLogger("LkEngine")

def FindLibrary(BuildDir, BuildConfig):
    """Find the generated Assimp library."""
    if SystemPlatform == "Windows":
        SearchDir = Path(BuildDir) / "lib" / BuildConfig
        Pattern = re.compile(r"assimp-vc\d+-mt(d?)\.lib$", re.IGNORECASE)
    else:
        SearchDir = Path(BuildDir) / "lib"
        Pattern = re.compile(r"libassimp\.a$", re.IGNORECASE)

    print(f"[Build-Assimp] SearchDir: {SearchDir}", flush=True)
    if not SearchDir.exists():
        print(f"[Build-Assimp] Library search directory does not exist: {SearchDir}", flush=True)
        return None

    for File in SearchDir.iterdir():
        if File.is_file() and Pattern.match(File.name):
            return File

    print(f"[Build-Assimp] Failed to locate Assimp library in: {SearchDir}", flush=True)
    return None

CurrentDir = Path(__file__).resolve().parent
ProjectRoot = CurrentDir

while ProjectRoot.name != "LkEngine" and ProjectRoot != ProjectRoot.parent:
    ProjectRoot = ProjectRoot.parent

if ProjectRoot.name != "LkEngine":
    print("[Build-Assimp] Failed to locate LkEngine root", flush=True)
    sys.exit(1)

AssimpDir = ProjectRoot / "External" / "Assimp" / "assimp"
BuildDir = AssimpDir / "build"
OutputDir = ProjectRoot / "External" / "Libraries"

if verbose: print(f"[Build-Assimp] Output dir: {OutputDir}", flush=True)
# Ensure the output directory exists.
os.makedirs(OutputDir, exist_ok=True)

SystemPlatform = platform.system()
IsPlatformWindows = SystemPlatform == "Windows"

Architecture = "x64"
try:
    Architecture = "x64" if platform.architecture()[0] == "64bit" else "x86"
except Exception as e:
    print(f"[Build-Assimp] Error occured when trying to determine system architecture, error: {e}")
    exit(1)

if verbose: print(f"[Build-Assimp] Architecture: {Architecture}")
if Architecture != "x64":
    print(f"[Build-Assimp] Warning: System architecture is not 64-bit")

LibraryExtension = ".lib" if IsPlatformWindows else ".a"
if verbose: print(f"[Build-Assimp] Build Dir: {Path(BuildDir).resolve()}", flush=True)

# TODO: Automatic selection of Debug/Release.
BuildAsDebug = True
BuildConfig = "debug" if BuildAsDebug else "release"

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

def RunCMakeConfiguration():
    """Run CMake configure command."""
    # TODO: Automatic assignment of architecture.
    result = None
    if verbose: print(f"[Build-Assimp] Current File Parent Directory: {Path(__file__).parent.resolve()}", flush=True)
    if verbose: print(f"[Build-Assimp] Current Working Directory: {Path().resolve()}", flush=True)
    if IsPlatformWindows:
        toolset = "v143"
        result = subprocess.check_call(["cmake", "..", "-A", f"{Architecture}", "-T", toolset] + CMakeFlags, cwd=BuildDir)
    else:
        result = subprocess.check_call(["cmake", ".."] + CMakeFlags, cwd=BuildDir)
    return result

def InvokeCMakeBuild():
    """Run CMake build command."""
    if verbose: print(f"[Build-Assimp] Running: 'cmake --build . --config {BuildConfig}'", flush=True)
    build_command = ["cmake", "--build", ".", "--config", BuildConfig]
    result = subprocess.check_call(build_command, cwd=BuildDir)
    return result

def CopyGeneratedFiles():
    """Copy the generated files to the desired locations."""

    LibraryFile = FindLibrary(BuildDir, BuildConfig)
    if LibraryFile is None:
        print(f"[Build-Assimp] LibraryFile is None, exiting", flush=True)
        sys.exit(1)

    DestinationFile = OutputDir / f"assimp{LibraryExtension}"
    print(f"[Build-Assimp] Copying static library\n  Source: {Path(LibraryFile).resolve()}\n  Destination: {Path(DestinationFile).resolve()}", flush=True)
    shutil.copy2(LibraryFile, DestinationFile)

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

def CleanUpAfterBuild():
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
        if verbose: print(f"[Build-Assimp] Creating build directory: {BuildDir}", flush=True)
    os.makedirs(BuildDir, exist_ok=True)

    try:
        CMakeConfigResult = RunCMakeConfiguration()
        if CMakeConfigResult != 0:
            print(f"[Build-Assimp] Error occured during the CMake configuration", flush=True)
            return 1 # Return non-zero to indicate error.

        BuildResult = InvokeCMakeBuild()
        CopyGeneratedFiles()
        if clean:
            CleanUpAfterBuild()
        else:
            if verbose: print(f"[Build-Assimp] Skipping clean after build ({BuildDir})")
        return BuildResult 

    except subprocess.CalledProcessError as e:
        Logger.error(f"[Assimp] Error occurred during the build process: {e}")
        return 1 # Return non-zero to indicate error.

if __name__ == "__main__":
    BuildAssimp()
