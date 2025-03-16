#-------------------------------------------------------------------------
# LkEngine Build Tool
#
#--------------------------------------------------------------------------
MAJOR_VERSION=1
MINOR_VERSION=0
PATCH_VERSION=0

import sys
import os
import platform
sys.path.append(os.path.join(os.path.dirname(__file__), "Tools")) # Add 'Tools' to path.
sys.path.append(os.path.join(os.path.dirname(__file__), "Tools/Module")) # Add 'Tools/Module' to path.

import argparse
import subprocess
import shutil
from colorama import Fore, Back, Style

import Tools.Module.ScriptUtils as Utils
from Tools.Module.ScriptUtils import ScriptLogger

def LOG_DEBUG(msg: str):
    """ Debug logging for the build script. """
    if tool_debug: print(f"[Debug] {msg}", flush=True)

def LOG_INFO(msg: str):
    """ Info logging for the build script. """
    print(f"[LkEngine-Build] {msg}", flush=True)

def GetBuildArgs(args=None):
    """ Get the argument passed after the '--' marker. """
    if args is None:
        # Exclude the script name by using sys.argv[1:].
        args = sys.argv[1:]
    try:
        index = args.index("--")
        return args[index + 1:]
    except ValueError:
        # The '--' marker was not found.
        return []

system_platform = platform.system()
project_solution = "LkEngine.sln" if system_platform == "Windows" else "Makefile"

tool_debug = False

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
argparser.add_argument('build_args', nargs=argparse.REMAINDER, help='Build arguments')

# Parse arguments.
args = argparser.parse_args()
tool_debug = args.tool_debug
verbose = args.verbose
build_config = args.config
generate_projects = args.generate_projects
cores = os.cpu_count() if args.j <= 0 else args.j

build_args = args.build_args
if build_args and build_args[0] == "--":
    build_args = build_args[1:]

passed_build_config = args.build or (any(sys.argv) == "--config") 
LOG_DEBUG(f"sys.argv: {sys.argv}")

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

    # TODO: Should do some validation of the arguments to make sure no preprocessor directive 
    #       is incorrectly passed to CFlags
    CFlags = ""
    use_cflags = False
    if len(build_args) > 0:
        CFlags = "CFLAGS+= " + " ".join(build_args)
        use_cflags = True
        LOG_DEBUG(f"CFlags: {CFlags}")

    Utils.PrintBanner(f"Running Make", 40, Fore.LIGHTCYAN_EX, '=')
    if use_cflags:
        subprocess.run(["make", f"-j{cores}", f"V={1 if verbose else 0}", CFlags, "LkEngine"], check=True)
        subprocess.run(["make", f"-j{cores}", f"V={1 if verbose else 0}", CFlags, "LkEditor"], check=True)
    else:
        subprocess.run(["make", f"-j{cores}", f"V={1 if verbose else 0}", "LkEngine"], check=True)
        subprocess.run(["make", f"-j{cores}", f"V={1 if verbose else 0}", "LkEditor"], check=True)
    print("", flush=True)
    Utils.PrintBanner(f"Build Complete", 40, Fore.LIGHTGREEN_EX, '=')

