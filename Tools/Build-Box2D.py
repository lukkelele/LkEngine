#--------------------------------------------------------------------------
# [LkEngine] Build_Box2D.py
#
# INTERNAL USE ONLY
#
# Build box2d library.
#--------------------------------------------------------------------------
import sys
import os
import platform
import subprocess
import shutil

from ScriptUtils import ScriptLogger
Logger = ScriptLogger("LkEngine")

# Paths to box2d, build and output directories.
box2d_dir = os.path.join("..", "External", "box2d")
build_dir = os.path.join(box2d_dir, "build")

CMakeFlags = [
	"-DBOX2D_UNIT_TESTS=OFF"
]

# Platform-specific options.
PlatformWindows = platform.system() == "Windows"

def BuildBox2D():
    """
    Build box2d library.
    """
    original_dir = os.getcwd()
    SetupResult = 1

    # Create the build directory.
    os.makedirs(build_dir, exist_ok=True) 

    try:
        # Navigate to the box2d build directory.
        os.chdir(build_dir)

        # Run CMake configuration.
        Logger.info("Running CMake configuration...")
        subprocess.check_call(["cmake", ".."] + CMakeFlags)

        # Build the project.
        SetupResult = subprocess.check_call(["cmake", "--build", "."])

    except Exception as e:
        Logger.error(f"[BOX2D] Error occurred: {e}")

    finally:
        # Change back to the original directory before exiting.
        os.chdir(original_dir)
        return SetupResult

if __name__ == "__main__":
    BuildBox2D()
