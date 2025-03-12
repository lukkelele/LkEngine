#--------------------------------------------------------------------------
# [LkEngine] SetupPremake.py
#
# Class to help with setting up and validating a Premake installation.
#--------------------------------------------------------------------------
import sys
import os
import glob
import platform
from pathlib import Path

import ScriptUtils as Utils

from ScriptUtils import ScriptLogger
Logger = ScriptLogger("LkEngine")

Platform = ("windows" if platform.system() == "Windows" else "linux")
ExecutableType = (".exe" if platform.system() == "Windows" else "")

class PremakeConfiguration:
    PremakeVersion = "5.0.0-beta2"
    PremakeZipUrls = f"https://github.com/premake/premake-core/releases/download/v{PremakeVersion}/premake-{PremakeVersion}-{Platform}.zip"
    PremakeLicenseUrl = "https://raw.githubusercontent.com/premake/premake-core/master/LICENSE.txt"
    PremakeDirectory = "External/Premake5/bin"

    @classmethod
    def Validate(cls):
        if (not cls.CheckIfPremakeInstalled()):
            Logger.warning("Premake is not installed.")
            return False
        Logger.info(f"Correct Premake located at {os.path.abspath(cls.PremakeDirectory)}")
        return True

    @classmethod
    def CheckIfPremakeInstalled(cls):
        PremakeExe = Path(f"{cls.PremakeDirectory}/premake5{ExecutableType}");
        if (not PremakeExe.exists()):
            return cls.InstallPremake()
        return True

    @classmethod
    def InstallPremake(cls):
        PremakePath = f"{cls.PremakeDirectory}/premake-{cls.PremakeVersion}-{Platform}.zip"
        Logger.info("Downloading {0:s} to {1:s}".format(cls.PremakeZipUrls, PremakePath))
        Utils.DownloadFile(cls.PremakeZipUrls, PremakePath)
        Logger.info(f"Extracting: {PremakePath}")
        Utils.UnzipFile(PremakePath, DeleteZipFile=True)
        Logger.info(f"Premake {cls.PremakeVersion} has been downloaded to '{cls.PremakeDirectory}'")

        PremakeLicensePath = f"{cls.PremakeDirectory}/LICENSE.txt"
        Logger.info("Downloading {0:s} to '{1:s}'".format(cls.PremakeLicenseUrl, PremakeLicensePath))
        Utils.DownloadFile(cls.PremakeLicenseUrl, PremakeLicensePath)
        Logger.info(f"Premake License file has been downloaded to '{cls.PremakeDirectory}'")

        # Clean up build resources before exit.
        AllowedExtensions = {'.exe', '.txt'}
   
		# Check if the directory exists 
        if os.path.isdir(cls.PremakeDirectory):
            for FilePath in glob.glob(os.path.join(cls.PremakeDirectory, '*')): 
                print(f"FILE: {FilePath}")
                # Check if it's a file and if it does not have an allowed extension 
                if os.path.isfile(FilePath) and os.path.splitext(FilePath)[1].lower() not in AllowedExtensions: 
                    try: 
                        os.remove(FilePath) 
                        #print(f"Deleted file: {FilePath}") 
                    except Exception as e: 
                        print(f"Error occured when deleting {FilePath}: {e}")
        return True
