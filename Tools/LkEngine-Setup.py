#--------------------------------------------------------------------------
# [LkEngine] Setup.py
#
# Build and setup required dependencies for the engine.
# 
# * Checks Python installation.
# * Checks Premake installation.
#--------------------------------------------------------------------------
import sys
import os
import subprocess
import platform

import ScriptUtils as Utils

import colorama
from colorama import Fore, Back, Style
colorama.init()

from ScriptUtils import ScriptLogger
Logger = ScriptLogger("LkEngine")

from PythonClass import PythonConfiguration
from PremakeClass import PremakeConfiguration

tools_dir = "Tools"
scripts_dir = os.path.join(f"{tools_dir}")
glfw_script = "Build-Glfw.py"
glad_script = "Build-Glad.py"
box2d_script = "Build-Box2D.py"

Utils.PrintBanner("LkEngine Setup", 60, Fore.CYAN, '=')

# Validate Python installation.
PythonConfiguration.IsInstalled()

Logger.info("Updating git submodules")
subprocess.call(["git", "submodule", "update", "--init", "--recursive"])

# TODO: Check current dir before invoking this
# Change to LkEngine root directory.
# Validate Premake installation.
Utils.ChangeDirectory("./..")
PremakeInstalled = PremakeConfiguration.Validate()

if (PremakeInstalled):
    # Build GLFW.
    print()
    Utils.PrintBanner("Building: Glfw", 40, Fore.WHITE, '-')
    SetupResult_Glfw = Utils.RunScript(glfw_script, script_dir=tools_dir)
    if SetupResult_Glfw != 0:
        Logger.error("Failed to build Glfw")
        sys.exit(1)
    Logger.success("Build complete: Glfw")

	# Build Glad.
    print()
    Utils.PrintBanner("Building: Glad", 40, Fore.WHITE, '-')
    SetupResult_Glad = Utils.RunScript(glad_script, script_dir=tools_dir)
    if SetupResult_Glad != 0:
        Logger.error("Failed to build Glad")
        sys.exit(1)
    Logger.success("Build complete: Glad")

	# Build Box2D.
    print()
    Utils.PrintBanner("Building: Box2D", 40, Fore.WHITE, '-')
    SetupResult_Box2D = Utils.RunScript(box2d_script, script_dir=tools_dir)
    if SetupResult_Box2D != 0:
        Logger.error("Failed to build Box2D")
        sys.exit(1)
    Logger.success("Build complete: Box2D")

    # Engine modules built, continue on with generating project files.
    Logger.info("All engine modules has been built")

    Logger.info("Continuing with project generation")
    if platform.system() == "Windows": 
        print()
        Utils.RunScript("GenerateProjects-Windows.py", tools_dir)

    print()
    Utils.PrintBanner("LkEngine Setup Complete", 60, Fore.GREEN, '=')
else:
    print()
    Utils.PrintBanner("ERROR: Premake is required to generate project files", 60, Fore.RED, '=')

