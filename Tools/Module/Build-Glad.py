#--------------------------------------------------------------------------
# [LkEngine] Build_Glad.py
# 
# INTERNAL USE ONLY
#
# Build and move generated header and source files to 'External/glad'.
#--------------------------------------------------------------------------
import sys
import os
import platform
import subprocess
import shutil

from ScriptUtils import ScriptLogger
Logger = ScriptLogger("LkEngine")

Profile = "core"
Generator = "c"
Api = "gl=4.6"
Extensions = [
    "GL_ARB_bindless_texture", 
    "GL_ARB_texture_storage", 
    "GL_ARB_texture_buffer_object", 
    "GL_ARB_depth_texture", 
    "GL_ARB_draw_instanced", 
    "GL_ARB_draw_buffers",
    "GL_ARB_texture_cube_map",
    "GL_ARB_texture_cube_map_array",
    "GL_ARB_texture_multisample",
    "GL_ARB_instanced_arrays",
    "GL_ARB_sample_shading",
    "GL_ARB_debug_output",
    "GL_ARB_timer_query",
    "GL_ARB_direct_state_access",
]

# Paths to Glad, build and output directories.
GladDir = os.path.abspath(os.path.join("..", "..", "External", "Glad"))
BuildDir = os.path.abspath(os.path.join(GladDir, "build"))
OutputPath = os.path.abspath(os.path.join(GladDir, "build"))
OutputDir = os.path.abspath(os.path.join("..", "..", "External", "Glad"))

# Ensure output directory exists.
os.makedirs(OutputDir, exist_ok=True)

# Platform-specific options.
PlatformWindows = (platform.system() == "Windows")

def IsGladModuleInstalled():
    """Checks if the Glad module is installed."""
    try:
        __import__("glad")
        return True
    except ImportError:
        return False

def InstallGladModule():
    """Install the Glad module."""
    try:
        subprocess.check_call([sys.executable, "-m", "pip", "install", "glad"])
        Logger.success("Glad module installed successfully")
    except subprocess.CalledProcessError as e:
        Logger.error(f"Failed to install GLAD module, error: {e}")
        sys.exit(1)

def GenerateGladLoader():
    """Generate GLAD loader."""
    try:
        command = [
            sys.executable, "-m", "glad",
            f"--profile={Profile}",
            f"--api={Api}",
            f"--extensions={','.join(Extensions) if Extensions else ''}",
            f"--generator={Generator}",
            f"--out-path={OutputPath}"
        ]
        subprocess.check_call(command)
    except subprocess.CalledProcessError as e:
        Logger.error(f"[GLAD] Generation of files failed, error: {e}")
        sys.exit(1)

def MoveGeneratedFiles():
    """Moves generated files from output directory to the Glad directory."""
    IncludeDir = os.path.join(OutputPath, "include")
    SourceDir = os.path.join(OutputPath, "src")

    DestInclude = os.path.join(GladDir, "include")
    DestSource = os.path.join(GladDir, "src")

    # Ensure destination directories exist and move files if it does.
    if os.path.exists(IncludeDir):
        # Remove old include directory if it exists.
        if os.path.exists(DestInclude):
            shutil.rmtree(DestInclude)
        shutil.move(IncludeDir, GladDir)

    if os.path.exists(SourceDir):
        # Remove old src directory if it exists.
        if os.path.exists(DestSource):
            shutil.rmtree(DestSource)
        shutil.move(SourceDir, GladDir)

    # Clean the output directory.
    if os.path.exists(OutputPath):
        shutil.rmtree(OutputPath)
    

def BuildGlad():
    """Generate glad loader files."""
    os.makedirs(BuildDir, exist_ok=True)
    try:
        if not IsGladModuleInstalled():
            InstallGladModule()
        BuildResult = GenerateGladLoader()
        MoveGeneratedFiles()
        return BuildResult
    except subprocess.CalledProcessError as e:
        Logger.error(f"[GLAD] Error occurred during the build process: {e}")
        return 1 # Return non-zero to indicate error.


if __name__ == "__main__":
    BuildGlad()
