#-------------------------------------------------------------------------
# [LkEngine] Build.py
#
# Build the engine.
#--------------------------------------------------------------------------

import argparse
import sys
import os
import platform
import subprocess

#ScriptLogger = Logger("LkEngine")

argparser = argparse.ArgumentParser(
    description='LkEngine Build Tool'
)

argparser.add_argument(
    '-c', '--clean',
    action='store_true',
    required=False,
    help='Clean build'
)

argparser.add_argument(
    '-g', '--generate-project',
    action='store_true',
    required=False,
    help='Generate project files'
)

argparser.add_argument(
    '--purge',
    action='store_true',
    required=False,
    help='Purge all intermediate and binary directories'
)

argparser.add_argument(
    '-v', '--verbose',
    action='store_true',
    default=False,
    required=False,
    help='Verbose build'
)

argparser.add_argument(
    '-j',
    type=int,
    default=-1,
    required=False,
    help='Number of cores to use for building with make'
)

argparser.add_argument(
    '--build-assimp',
    action='store_true',
    default=False,
    required=False,
    help='Build the assimp library'
)

args = argparser.parse_args()
print(f"Platform: {platform.system()}")

if args.generate_project:
    result = subprocess.run([sys.executable, "GenerateProjects.py"], check=True)
    subprocess.run(["make", "clean"], check=True)
if args.clean:
    subprocess.run(["make", "clean"], check=True)
if args.build_assimp:
    result = subprocess.run([sys.executable, "Tools/Module/Build-Assimp.py"], check=True)
    exit(0)

cores = os.cpu_count() if args.j == -1 else args.j
verbose = 1 if args.verbose else 0

# rm -rf Binaries Intermediate Makefile LkEngine/Makefile LkEditor/Makefile External/*/Makefile

#subprocess.run(["make", f"-j{cores}", f"V={verbose}"], check=True)
subprocess.run(["make", f"-j{cores}", f"V={verbose}", "LkEngine"], check=True)
subprocess.run(["make", f"-j{os.cpu_count()}", f"V={verbose}", "LkEditor"], check=True)
#subprocess.run(["make", f"-j{cores}", f"V={verbose}", "LkEditor"], check=True)

