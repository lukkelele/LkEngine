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

# Glad generation arguments.
profile = "core"
generator = "c"
api = "gl=4.6"

# Paths to Glad, build and output directories.
glad_dir = os.path.join("..", "External", "glad")
build_dir = os.path.join(glad_dir, "build")
output_dir = os.path.join("..", "External", "glad")
output_path = os.path.join(glad_dir, "output")

# Ensure output directory exists.
os.makedirs(output_dir, exist_ok=True)

# Platform-specific options.
PlatformWindows = platform.system() == "Windows"

def _IsGladModuleInstalled():
    """Checks if the Glad module is installed."""
    try:
        __import__("glad")
        return True
    except ImportError:
        return False

def _InstallGladModule():
    """Install the Glad module."""
    try:
        subprocess.check_call([sys.executable, "-m", "pip", "install", "glad"])
        Logger.success("Glad module installed successfully.")
    except subprocess.CalledProcessError as e:
        Logger.error(f"Failed to install GLAD module, error: {e}")
        sys.exit(1)

def _GenerateGladLoader():
    """Generate GLAD loader."""
    try:
        command = [
            sys.executable, "-m", "glad",
            f"--profile={profile}",
            f"--api={api}",
            f"--generator={generator}",
            f"--out-path={output_path}"
        ]
        subprocess.check_call(command)
        #Logger.info(f"GLAD generated successfully in '{output_path}' directory.")
    except subprocess.CalledProcessError as e:
        Logger.error(f"[GLAD] Generation of files failed: {e}")
        sys.exit(1)

def _MoveGeneratedFiles():
    """Moves generated files from output directory to the Glad directory."""
    src_include = os.path.join(output_path, "include")
    src_src = os.path.join(output_path, "src")

    dest_include = os.path.join(glad_dir, "include")
    dest_src = os.path.join(glad_dir, "src")

    # Ensure destination directories exist and move files if it does.
    #Logger.info("Moving the generated Glad files out from build directory")
    if os.path.exists(src_include):
        if os.path.exists(dest_include):
            # Remove old include directory if it exists.
            shutil.rmtree(dest_include)
        shutil.move(src_include, glad_dir)
        #Logger.info(f"Moved 'include' to '{glad_dir}'")

    if os.path.exists(src_src):
        if os.path.exists(dest_src):
            # Remove old src directory if it exists.
            shutil.rmtree(dest_src)
        shutil.move(src_src, glad_dir)
        #Logger.info(f"Moved 'src' to '{glad_dir}'")

    # Cleanup: Remove the output directory if it’s empty.
    if os.path.exists(output_path):
        shutil.rmtree(output_path)
        #Logger.info(f"Cleaned up '{output_path}' directory")
    

def BuildGlad():
    os.makedirs(build_dir, exist_ok=True)
    try:
        if not _IsGladModuleInstalled():
            Logger.warning("Glad module not found, installing it")
            _InstallGladModule()
        _GenerateGladLoader()
        _MoveGeneratedFiles()
        return True
    except subprocess.CalledProcessError as e:
        Logger.error(f"[GLAD] Error occurred during the build process: {e}")
        return False

if __name__ == "__main__":
    BuildGlad()
