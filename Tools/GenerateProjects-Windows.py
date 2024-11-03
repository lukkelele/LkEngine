#--------------------------------------------------------------------------
# [LkEngine] GenerateProjects-Windows.py
#
# Generate project files.
#--------------------------------------------------------------------------

import os
import subprocess
from colorama import Fore, Back, Style

import ScriptUtils as Utils
from PythonClass import PythonConfiguration
from PremakeClass import PremakeConfiguration

PythonConfiguration.ValidatePackages()

# TODO: Validate current path before doing this chdir call here.
# Enter LkEngine root directory.
os.chdir("../")

Utils.PrintBanner("Generating Visual Studio 2022 solution", 60, Fore.WHITE, '=')
subprocess.call(["External/premake/bin/premake5.exe", "vs2022", "--file=LkEngine.lua"])
