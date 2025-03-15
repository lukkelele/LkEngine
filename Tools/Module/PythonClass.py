import os
import sys
sys.path.append(os.path.dirname(__file__))

import subprocess
import importlib.util as importlib_util
import pkg_resources # FIXME: Deprecated

from ScriptUtils import ScriptLogger
Logger = ScriptLogger("LkEngine")

class PythonConfiguration:
    @classmethod
    def IsInstalled(cls):
        if not cls.__ValidatePython():
            print(f"Python validation failed")
            return # Cannot validate further.

        for packageName in ["requests"]:
            if not cls.__ValidatePackage(packageName):
                print(f"Error for package name: {packageName}")
                return # Cannot validate further.

    @classmethod
    def __ValidatePython(cls, versionMajor = 3, versionMinor = 3):
        if sys.version is not None:
            Logger.info("Python version {0:d}.{1:d}.{2:d} detected.".format( \
                sys.version_info.major, sys.version_info.minor, sys.version_info.micro))
            if sys.version_info.major < versionMajor or (sys.version_info.major == versionMajor and sys.version_info.minor < versionMinor):
                Logger.error("Python version too low, expected version {0:d}.{1:d} or higher.".format( \
                    versionMajor, versionMinor))
                return False
            return True

    @classmethod
    def __ValidatePackage(cls, packageName):
        if importlib_util.find_spec(packageName) is None:
            return cls.__InstallPackage(packageName)
        return True

    @classmethod
    def __InstallPackage(cls, packageName):
        PermissionGranted = False
        while not PermissionGranted:
            reply = str(input("Install Python package '{0:s}'? [Y/N]: ".format(packageName))).lower().strip()[:1]
            if reply == 'n':
                return False
            PermissionGranted = (reply == 'y')
        
        Logger.info(f"Installing {packageName} module...")
        subprocess.check_call(['python', '-m', 'pip', 'install', packageName])

        return cls.__ValidatePackage(packageName)

    def InstallPackage(package): 
        print(f"Installing {package} module...") 
        subprocess.check_call(['python', '-m', 'pip', 'install', package])

    def ValidatePackage(package): 
        required = { package } 
        installed = {pkg.key for pkg in pkg_resources.working_set} 
        missing = required - installed
        if missing: 
            InstallPackage(package)

    @classmethod
    def ValidatePackages(cls): 
        cls.ValidatePackage('setuptools')
        cls.ValidatePackage('requests') 
        cls.ValidatePackage('fake-useragent')
        cls.ValidatePackage('colorama')

if __name__ == "__main__":
    # Check python installation if run as a standalone script.
    PythonConfiguration.IsInstalled()
