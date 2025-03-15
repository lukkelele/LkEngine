#-------------------------------------------------------------------------
# LkEngine Build Tool
#
#--------------------------------------------------------------------------
MAJOR_VERSION=1
MINOR_VERSION=0
PATCH_VERSION=0

import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), "Tools")) # Add 'Tools' to path.
sys.path.append(os.path.join(os.path.dirname(__file__), "Tools/Module")) # Add 'Tools/Module' to path.

import argparse
import platform
import subprocess
import shutil

from colorama import Fore, Back, Style

tool_debug = False
system_platform = platform.system()

project_solution = "LkEngine.sln" if system_platform == "Windows" else "Makefile"

def LOG_DEBUG(msg: str):
    """ Debug logging for the build script. """
    if tool_debug: print(f"[Debug] {msg}", flush=True)

def LOG_INFO(msg: str):
    """ Info logging for the build script. """
    print(f"[LkEngine-Build] {msg}", flush=True)

import Tools.Module.ScriptUtils as Utils
from Tools.Module.ScriptUtils import ScriptLogger

argparser = argparse.ArgumentParser(
    description=f"LkEngine Build Tool v{MAJOR_VERSION}.{MINOR_VERSION}.{PATCH_VERSION}",
    formatter_class=argparse.ArgumentDefaultsHelpFormatter
)
argparser.add_argument('-c', '--config', type=str, required=False, default="Debug", help='Build configuration')
argparser.add_argument('-g', '--generate-projects', action='store_true', required=False, help='Generate project files')
argparser.add_argument('--clean', action='store_true', required=False, help='Clean command (i.e make clean)')
argparser.add_argument('--purge', action='store_true', required=False, help='Remove all intermediate and binary directories')
argparser.add_argument('-v', '--verbose', action='store_true', default=False, required=False, help='Enable verbose build')
argparser.add_argument('--tool-debug', action='store_true', default=False, required=False, help='Enable debugging for the build script')
argparser.add_argument('-j', type=int, default=-1, required=False, help='Number of cores to use for the build')
argparser.add_argument('--build', action='store_true', default=None, required=False, help='Build the engine')
argparser.add_argument('--build-assimp', action='store_true', default=False, required=False, help='Build the assimp library')

# Parse arguments.
args = argparser.parse_args()
tool_debug = args.tool_debug
verbose = args.verbose
build_config = args.config
generate_projects = args.generate_projects
cores = os.cpu_count() if args.j <= 0 else args.j
LOG_DEBUG(f"sys.argv: {sys.argv}")
LOG_DEBUG(f"build arg: {args.build}")
passed_build_config = args.build or (any(sys.argv) == "--config") 

# Print banner.
Utils.PrintBanner(f"LkEngine Build Tool ({system_platform})", 60, Fore.LIGHTBLUE_EX, '=')
if len(sys.argv) == 1 or any(sys.argv) == "-c" or any(sys.argv) == "--config":
    print(f" * Configuration: {build_config}", flush=True)
    print(f" * Cores: {cores}", flush=True)
print("", flush=True)

generate_projects_exe = "GenerateProjects.py"
if not os.path.exists(generate_projects_exe):
    generate_projects_exe = "Tools/GenerateProjects.py"
    if not os.path.exists(generate_projects_exe):
        Utils.PrintBanner(f"Failed to find GenerateProjects.py", 40, Fore.RED, '=')
        exit(1)

# Generate project files they are missing.
if not os.path.exists(project_solution):
    generate_projects = True

# Platform: Windows
if system_platform == "Windows":
    if args.purge:
        Utils.PrintBanner(f"Purging LkEngine", 40, Fore.LIGHTRED_EX, '=')
        files_to_remove = [
            "LkEngine.sln",
            os.path.join("LkEngine", "LkEngine.sln"),
            os.path.join("LkEditor", "LkEditor.sln")
        ]
        dirs_to_remove = [ "Binaries", "Intermediate" ]
        LOG_INFO(f"Files to purge: {len(files_to_remove)}")
        LOG_INFO(f"Directories to purge: {len(dirs_to_remove)}")
        for d in dirs_to_remove:
            if os.path.exists(d):
                shutil.rmtree(d)
                LOG_DEBUG(f"Removed directory: {d}")
        for f in files_to_remove:
            if os.path.exists(f):
                os.remove(f)
                LOG_DEBUG(f"Removed file: {f}")
        print("", flush=True)
        Utils.PrintBanner(f"Purge Complete", 40, Fore.LIGHTGREEN_EX, '=')
        exit(0)
    if generate_projects:
        result = subprocess.run([sys.executable, generate_projects_exe], check=True)
        subprocess.run(["MSBuild.exe", "LkEngine.sln", "-t:Clean"], check=True)
        if args.build != True or not passed_build_config:
            exit(0)
    if args.build_assimp:
        result = subprocess.run([sys.executable, "Tools/Module/Build-Assimp.py"], check=True)
        exit(0)
    if args.clean:
        subprocess.run(["MSBuild.exe", "LkEngine.sln", "-t:Clean"], check=True)
        exit(0)
         
    Utils.PrintBanner(f"Running MSBuild", 40, Fore.LIGHTCYAN_EX, '=')
    subprocess.run(["MSBuild.exe", "LkEngine.sln", f"-p:Configuration={build_config}", f"-maxCpuCount:{cores}"], check=True)
    print("", flush=True)
    Utils.PrintBanner(f"Build Complete", 40, Fore.LIGHTGREEN_EX, '=')

# Platform: Linux
elif system_platform == "Linux":
    if args.purge:
        Utils.PrintBanner(f"Purging LkEngine", 40, Fore.LIGHTRED_EX, '=')
        result = subprocess.run(["rm", "-rfv", "Binaries Intermediate Makefile LkEngine/Makefile LkEditor/Makefile External/*/Makefile"], check=True)
        Utils.PrintBanner(f"Purge Complete", 40, Fore.LIGHTGREEN_EX, '=')
        exit(0)
    if generate_projects:
        result = subprocess.run([sys.executable, generate_projects_exe], check=True)
        subprocess.run(["make", "clean"], check=True)
        if args.build != True:
            exit(0)
        elif not passed_build_config:
            exit(0)
    if args.clean:
        Utils.PrintBanner(f"Cleaning", 40, Fore.LIGHTWHITE_EX, '=')
        subprocess.run(["make", "clean"], check=True)
        exit(0)
    if args.build_assimp:
        result = subprocess.run([sys.executable, "Tools/Module/Build-Assimp.py"], check=True)
        exit(0)

    Utils.PrintBanner(f"Running Make", 40, Fore.LIGHTCYAN_EX, '=')
    subprocess.run(["make", f"-j{cores}", f"V={verbose}", "LkEngine"], check=True)
    subprocess.run(["make", f"-j{os.cpu_count()}", f"V={verbose}", "LkEditor"], check=True)
    print("", flush=True)
    Utils.PrintBanner(f"Build Complete", 40, Fore.LIGHTGREEN_EX, '=')

