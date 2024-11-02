#--------------------------------------------------------------------------
# [LkEngine] Setup.py
#
# Setup engine environemnt, building all required dependencies libraries.
#--------------------------------------------------------------------------

import sys
import os
import platform
import subprocess
import shutil

tools_dir = "Tools"

from Tools.Logger import Logger
ScriptLogger = Logger("LkEngine")

scripts_dir = os.path.join(f"{tools_dir}")
glfw_script = "Build_Glfw.py"
glad_script = "Build_Glad.py"

def run_script(script_name):
    """Run a script and capture its output."""
    try:
        result = subprocess.run([sys.executable, script_name], check=True, cwd=scripts_dir)
        ScriptLogger.info(f"{script_name} completed successfully.")
    except subprocess.CalledProcessError as e:
        ScriptLogger.error(f"Error occurred while running {script_name}: {e}")
        sys.exit(1)

def main():
    """
    Install required dependencies for the engine
    by running the build scripts for the modules.
    """
    ScriptLogger.info("Starting setup")

    # Build Glfw.
    ScriptLogger.info("Installing: Glfw")
    run_script(glfw_script)

    # Build Glad.
    ScriptLogger.info("Installing: Glad")
    run_script(glad_script)

    ScriptLogger.success("Engine setup complete!")
    print()

if __name__ == "__main__":
    main()
