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

# Directory paths.
Box2DDir = os.path.join("..", "..", "External", "box2d")
BuildDir = os.path.join(Box2DDir, "build")

CMakeFlags = [
	"-DBOX2D_UNIT_TESTS=OFF"
]

# Platform-specific options.
PlatformWindows = platform.system() == "Windows"

def BuildBox2D():
    """Build box2d library."""
    OriginalDir = os.getcwd()
    SetupResult = 1

    # TODO: Add way to clean build dir entirely without exceptions (from _deps).

    # Create the build directory.
    os.makedirs(BuildDir, exist_ok=True) 

    try:
        # Navigate to the box2d build directory.
        os.chdir(BuildDir)

        # Run CMake configuration.
        subprocess.check_call(["cmake", ".."] + CMakeFlags)

        # Build the project.
        SetupResult = subprocess.check_call(["cmake", "--build", "."])

    except Exception as e:
        Logger.error(f"[BOX2D] Error occurred: {e}")

    finally:
        # Change back to the original directory before exiting.
        os.chdir(OriginalDir)
        return SetupResult

if __name__ == "__main__":
    BuildBox2D()
