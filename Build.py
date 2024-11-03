#--------------------------------------------------------------------------
# [LkEngine] Build.py
#
# Build engine.
#--------------------------------------------------------------------------

import sys
import os
import platform
import subprocess
import shutil

tools_dir = "Tools"

from Tools.Logger import Logger
ScriptLogger = Logger("LkEngine")

def RunPythonScript(script_name):
    """Run a python script and capture its output."""
    try:
        result = subprocess.run([sys.executable, script_name], check=True, cwd=scripts_dir)
        ScriptLogger.info(f"{script_name} completed successfully.")
    except subprocess.CalledProcessError as e:
        ScriptLogger.error(f"Error occurred while running {script_name}: {e}")
        sys.exit(1)

def main():
    """
    Build LkEngine.
    """
    ScriptLogger.info("Starting LkEngine build")

    ScriptLogger.success("LkEngine build complete")

if __name__ == "__main__":
    main()
