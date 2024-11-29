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
from pathlib import Path

bPlatformActionRunner = (os.environ.get("CI") == "true")

def IsPythonPackageInstalled(PackageName):
    """Check if a Package is installed using pip."""
    try:
        subprocess.check_call(['python', '-m', 'pip', 'show', PackageName], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        return True
    except subprocess.CalledProcessError:
        return False

RequiredPythonDependencies = [
    "setuptools",
    "requests",
    "fake-useragent",
    "colorama",
]
# Install Python dependencies.
for RequiredPythonPackage in RequiredPythonDependencies:
    if not IsPythonPackageInstalled(RequiredPythonPackage):
        subprocess.check_call(['python', '-m', 'pip', 'install', RequiredPythonPackage])

import colorama
from colorama import Fore, Back, Style
colorama.init(strip="true" if bPlatformActionRunner else "false")

# Add 'Tools/Module' to path.
sys.path.append(os.path.join(os.path.dirname(__file__), "Module"))

import Module.ScriptUtils as Utils
from Module.ScriptUtils import ScriptLogger
Logger = ScriptLogger("LkEngine")

from Module.PythonClass import PythonConfiguration
from Module.PremakeClass import PremakeConfiguration

ToolsDir = "Tools"
ModuleDir = os.path.join(f"{ToolsDir}", "Module")
BuildScript_Glad = "Build-Glad.py"
BuildScript_Assimp = "Build-Assimp.py"

Packages = [
    { "name": "Glad",   "script":  BuildScript_Glad,   "color":  Fore.WHITE },
    { "name": "Assimp", "script":  BuildScript_Assimp, "color":  Fore.WHITE },
]
PackageBannerSize = 38
PackageBannerChar = '='

Utils.PrintBanner("LkEngine Setup", 60, Fore.WHITE, '=')

# Validate Python installation.
PythonConfiguration.IsInstalled()

Logger.info("Updating git submodules")
subprocess.call(["git", "submodule", "update", "--init", "--recursive"])

# Validate Premake installation, change to LkEngine root directory if not already there.
# Check so platform is not a Github action runner before attempting to re-adjust current path.
# The action runner pathing is a bit different, with two 'LkEngine' after each other (i.e 'LkEngine/LkEngine').
CurrentPath = Path.cwd()
ParentPath = CurrentPath.parents[0]
if ParentPath.name == "LkEngine" and not bPlatformActionRunner:
    os.chdir("./..")

IsPremakeInstalled = PremakeConfiguration.Validate()
if (IsPremakeInstalled):
    # Create 'Libraries' directory which is the folder for all built static libraries.
    os.makedirs("External/Libraries", exist_ok=True)

    # Install packages. 
    for Package in Packages: 
        print() 
        Utils.PrintBanner(f"Building: {Package['name']}", PackageBannerSize, Package["color"], PackageBannerChar) 
        BuildResult = Utils.RunScript(Package["script"], script_dir=ModuleDir) 

        if BuildResult != 0: 
            Utils.PrintBanner(f"Failed to build: {Package['name']}", PackageBannerSize, Fore.RED, PackageBannerChar) 
            sys.exit(1) 

        Utils.PrintBanner(f"Build Complete: {Package['name']}", PackageBannerSize, Fore.GREEN, PackageBannerChar)

    # Engine modules built, continue on with generating project files.
    Logger.info("Built all engine modules")

    if platform.system() == "Windows": 
        print()
        Utils.RunScript("GenerateProjects-Windows.py", ToolsDir)

    print("\n")
    Utils.PrintBanner("LkEngine Setup Complete", 60, Fore.GREEN, '=')
else:
    print()
    Utils.PrintBanner("ERROR: Premake is required to generate project files", 60, Fore.RED, '=')

