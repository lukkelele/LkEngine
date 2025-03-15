#--------------------------------------------------------------------------
# [LkEngine] GenerateProjects.py
#
# Generate project files.
#--------------------------------------------------------------------------
import sys
import os
import subprocess
from pathlib import Path
from platform import system
from colorama import Fore, Back, Style

# Add 'Tools/Module' to path.
#sys.path.append(os.path.join(os.path.dirname(__file__), "Tools"))
sys.path.append(os.path.join(os.path.dirname(__file__), "Tools/Module"))

SystemPlatform = system()

import Tools.Module.ScriptUtils as Utils
from Tools.Module.PythonClass import PythonConfiguration
from Tools.Module.PremakeClass import PremakeConfiguration

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

