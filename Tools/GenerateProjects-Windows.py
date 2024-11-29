#--------------------------------------------------------------------------
# [LkEngine] GenerateProjects-Windows.py
#
# Generate project files.
#--------------------------------------------------------------------------

import sys
import os
import subprocess
from pathlib import Path
from colorama import Fore, Back, Style

# Add 'Tools/Module' to path.
sys.path.append(os.path.join(os.path.dirname(__file__), "Module"))

import Module.ScriptUtils as Utils
from Module.PythonClass import PythonConfiguration
from Module.PremakeClass import PremakeConfiguration

PythonConfiguration.ValidatePackages()

# Change to project root if not already there.
bInProjectRoot = (Path.is_dir(Path("LkEngine/Source")) and Path.is_dir(Path("External")))
if not bInProjectRoot:
	os.chdir("..")
	if not Path.is_dir(Path("LkEngine/Source")):
		raise Exception(f"Failed to find LkEngine project root directory")

Utils.PrintBanner("Generating Project: LkEngine", 60, Fore.WHITE, '=')
subprocess.call(["External/premake/bin/premake5.exe", "vs2022", "--file=LkEngine.lua"])
