#-------------------------------------------------------------------------
# LkEngine Build Tool
#
#--------------------------------------------------------------------------
import argparse
import sys
import os
import platform
import subprocess

from colorama import Fore, Back, Style

system_platform = platform.system()

# Add 'Tools/Module' to path.
sys.path.append(os.path.join(os.path.dirname(__file__), "Tools/Module"))

import Tools.Module.ScriptUtils as Utils
from Tools.Module.ScriptUtils import ScriptLogger

argparser = argparse.ArgumentParser(description='LkEngine Build Tool')
argparser.add_argument('-c', '--config', type=str, required=False, default="Debug", help='Build configuration')
argparser.add_argument('--clean', action='store_true', required=False, help='Clean build')
argparser.add_argument('-g', '--generate-project', action='store_true', required=False, help='Generate project files')
argparser.add_argument('--purge', action='store_true', required=False, help='Purge all intermediate and binary directories')
argparser.add_argument('-v', '--verbose', action='store_true', default=False, required=False, help='Verbose build')
argparser.add_argument('-j', type=int, default=-1, required=False, help='Number of cores to use for building with make')
argparser.add_argument('--build-assimp', action='store_true', default=False, required=False, help='Build the assimp library')

args = argparser.parse_args()
verbose = args.verbose
build_config = args.config
cores = os.cpu_count() if args.j <= 0 else args.j

Utils.PrintBanner(f"LkEngine Build Tool ({system_platform})", 60, Fore.LIGHTBLUE_EX, '=')
print(f" * Build Configuration: {build_config}", flush=True)
print(f" * Cores: {cores}", flush=True)
print("", flush=True)

if system_platform == "Windows":
    if args.generate_project:
        result = subprocess.run([sys.executable, "GenerateProjects.py"], check=True)
        subprocess.run(["MSBuild.exe", "LkEngine.sln", "-t:Clean"], check=True)
        exit(0)
    if args.build_assimp:
        result = subprocess.run([sys.executable, "Tools/Module/Build-Assimp.py"], check=True)
        exit(0)
    if args.clean:
        subprocess.run(["MSBuild.exe", "LkEngine.sln", "-t:Clean"], check=True)
        exit(0)
        
    Utils.PrintBanner(f"Starting MSBuild", 40, Fore.LIGHTCYAN_EX, '=')
    subprocess.run(["MSBuild.exe", "LkEngine.sln", f"-p:Configuration={build_config}", f"-maxCpuCount:{cores}"], check=True)
    print("", flush=True)
    Utils.PrintBanner(f"Build Complete", 40, Fore.LIGHTGREEN_EX, '=')

elif system_platform == "Linux":
    if args.purge:
        result = subprocess.run(["bash", "rm -rf Binaries Intermediate Makefile LkEngine/Makefile LkEditor/Makefile External/*/Makefile"], check=True)
        exit(0)
    if args.generate_project:
        result = subprocess.run([sys.executable, "GenerateProjects.py"], check=True)
        subprocess.run(["make", "clean"], check=True)
        exit(0)
    if args.clean:
        subprocess.run(["make", "clean"], check=True)
    if args.build_assimp:
        result = subprocess.run([sys.executable, "Tools/Module/Build-Assimp.py"], check=True)
        exit(0)

    subprocess.run(["make", f"-j{cores}", f"V={verbose}", "LkEngine"], check=True)
    subprocess.run(["make", f"-j{os.cpu_count()}", f"V={verbose}", "LkEditor"], check=True)

