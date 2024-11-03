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

# Change to LkEngine root directory if not already there.
CurrentPath = Path.cwd()
ParentPath = CurrentPath.parents[0]
if ParentPath.name == "LkEngine":
    os.chdir("./..")

Utils.PrintBanner("Generating Visual Studio 2022 solution", 60, Fore.WHITE, '=')
subprocess.call(["External/premake/bin/premake5.exe", "vs2022", "--file=LkEngine.lua"])
