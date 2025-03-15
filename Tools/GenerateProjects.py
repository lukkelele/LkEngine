#--------------------------------------------------------------------------
# [LkEngine] GenerateProjects.py
#
# Generate project files.
#--------------------------------------------------------------------------
from platform import system
SystemPlatform = system()

import sys
import os
file_dirname = os.path.basename(os.path.dirname(__file__))
if file_dirname == "LkEngine":
    sys.path.append(os.path.join(os.path.dirname(__file__), "Tools/Module"))
elif file_dirname == "Tools":
    sys.path.append(os.path.dirname(__file__))

import subprocess
from pathlib import Path
from colorama import Fore, Back, Style

if file_dirname == "LkEngine":
    import Tools.Module.ScriptUtils as Utils
    from Tools.Module.PythonClass import PythonConfiguration
    from Tools.Module.PremakeClass import PremakeConfiguration
elif file_dirname == "Tools":
    import Module.ScriptUtils as Utils
    from Module.PythonClass import PythonConfiguration
    from Module.PremakeClass import PremakeConfiguration

PythonConfiguration.ValidatePackages()

# Change to project root if not already there.
bInProjectRoot = (Path.is_dir(Path("LkEngine/Source")) and Path.is_dir(Path("External")))
if not bInProjectRoot:
	os.chdir("..")
	if not Path.is_dir(Path("LkEngine/Source")):
		raise Exception("Failed to find LkEngine project root directory")

Utils.PrintBanner("Generating Project: LkEngine", 60, Fore.LIGHTWHITE_EX, '=')
if SystemPlatform == "Windows":
    subprocess.call(["External/Premake5/bin/premake5.exe", "vs2022", "--file=LkEngine.lua"])
elif SystemPlatform == "Linux":
    subprocess.call(["External/Premake5/bin/premake5", "gmake", "--file=LkEngine.lua"])
else:
	raise Exception("Unsupported platform")

