#--------------------------------------------------------------------------
# [LkEngine] GenerateProject.py
#
# Generate project files.
#
# TODO: Needs argparsing and the ability to select generation format.
#       Should be able to select from gmake and vs2022 etc.
#--------------------------------------------------------------------------

import sys
import os
import platform
import subprocess
import shutil

tools_dir = "Tools"
scripts_dir = os.path.join(f"{tools_dir}")

from Tools.Logger import Logger
ScriptLogger = Logger("LkEngine")

IsWindowsPlatform = platform.system() == "Windows"

ScriptLogger.info(f"Scripts dir: {scripts_dir}")

def RunScript(script_name):
    """Run a script and capture its output."""
    script_path = os.path.join(scripts_dir, script_name)
    if not os.path.isfile(script_path):
        ScriptLogger.error(f"Script not found: {script_name} (in directory: {scripts_dir}")
        sys.exit(1)

    try:
        result = subprocess.run([script_path], check=True)
        ScriptLogger.info(f"{script_name} completed successfully.")
    except subprocess.CalledProcessError as e:
        ScriptLogger.error(f"Error occurred while running {script_name}: {e}")
        sys.exit(1)

def main():
    """
    Generate project files.
    """
    ScriptLogger.info(f"Generating project files on {platform.system()}")
    if IsWindowsPlatform:
        GenerateScript = "ProjectGeneration_Windows.bat"
    else:
        GenerateScript = "ProjectGeneration_Linux.sh"

    ScriptLogger.info(f"Generate script: {GenerateScript}")
    RunScript(GenerateScript)

    ScriptLogger.success("Project file generation complete!")

if __name__ == "__main__":
    main()
