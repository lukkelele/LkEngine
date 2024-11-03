#--------------------------------------------------------------------------
# [LkEngine] ScriptUtils.py
#
#
#--------------------------------------------------------------------------
import sys
import subprocess
import os
import winreg

import requests
import time
import urllib

from zipfile import ZipFile

from colorama import Fore, Back, Style

class Colors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

class ScriptLogger:
    def __init__(self, name="Logger"):
        self.name = name

    def _format_message(self, level, message, color):
        return f"{color}[{self.name}] [{level}] {message}{Colors.ENDC}"

    def log(self, message, color=Colors.HEADER):
        return f"{color}[{self.name}] {message}{Colors.ENDC}"

    def info(self, message):
        print(self._format_message("INFO", message, Colors.OKBLUE))

    def success(self, message):
        print(self._format_message("SUCCESS", message, Colors.OKGREEN))

    def warning(self, message):
        print(self._format_message("WARNING", message, Colors.WARNING))

    def error(self, message):
        print(self._format_message("ERROR", message, Colors.FAIL))

def GetSystemEnvironmentVariable(EnvName):
    Key = winreg.CreateKey(winreg.HKEY_LOCAL_MACHINE, r"System\CurrentControlSet\Control\Session Manager\Environment")
    try:
        return winreg.QueryValueEx(Key, EnvName)[0]
    except:
        return None

def GetUserEnvironmentVariable(EnvName):
    Key = winreg.CreateKey(winreg.HKEY_CURRENT_USER, r"Environment")
    try:
        return winreg.QueryValueEx(Key, EnvName)[0]
    except:
        return None

def ChangeDirectory(directory_path):
    """Validate and change to the specified directory if valid."""
    if os.path.isdir(directory_path):
        os.chdir(directory_path)
        return True
    else:
        return False 

def RunScript(script_name, script_dir="./"):
    """Run a script and capture its output."""
    try:
        result = subprocess.run([sys.executable, script_name], check=True, cwd=script_dir)
        return result.returncode 
    except subprocess.CalledProcessError as e:
        return 1

def DownloadFile(url, filepath):
    path = filepath
    filepath = os.path.abspath(filepath)
    os.makedirs(os.path.dirname(filepath), exist_ok=True)
            
    if (type(url) is list):
        for url_option in url:
            print("Downloading", url_option)
            try:
                DownloadFile(url_option, filepath)
                return
            except urllib.error.URLError as e:
                print(f"URL Error encountered: {e.reason}. Proceeding with backup...\n\n")
                os.remove(filepath)
                pass
            except urllib.error.HTTPError as e:
                print(f"HTTP Error  encountered: {e.code}. Proceeding with backup...\n\n")
                os.remove(filepath)
                pass
            except:
                print(f"Something went wrong. Proceeding with backup...\n\n")
                os.remove(filepath)
                pass
        raise ValueError(f"Failed to download {filepath}")
    if not(type(url) is str):
        raise TypeError("Argument 'url' must be of type list or string")

    with open(filepath, 'wb') as f:
        headers = {'User-Agent': "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/83.0.4103.97 Safari/537.36"}
        response = requests.get(url, headers=headers, stream=True)
        total = response.headers.get('content-length')

        if total is None:
            f.write(response.content)
        else:
            downloaded = 0
            total = int(total)
            startTime = time.time()
            for data in response.iter_content(chunk_size=max(int(total/1000), 1024*1024)):
                downloaded += len(data)
                f.write(data)
                
                try:
                    done = int(50*downloaded/total) if downloaded < total else 50
                    percentage = (downloaded / total) * 100 if downloaded < total else 100
                except ZeroDivisionError:
                    done = 50
                    percentage = 100
                elapsedTime = time.time() - startTime
                try:
                    avgKBPerSecond = (downloaded / 1024) / elapsedTime
                except ZeroDivisionError:
                    avgKBPerSecond = 0.0

                avgSpeedString = '{:.2f} KB/s'.format(avgKBPerSecond)
                if (avgKBPerSecond > 1024):
                    avgMBPerSecond = avgKBPerSecond / 1024
                    avgSpeedString = '{:.2f} MB/s'.format(avgMBPerSecond)
                sys.stdout.write('\r[{}{}] {:.2f}% ({})     '.format('█' * done, '.' * (50-done), percentage, avgSpeedString))
                sys.stdout.flush()
    sys.stdout.write('\n')

def UnzipFile(filepath, DeleteZipFile=True):
    ZipFilePath = os.path.abspath(filepath) # get full path of files
    ZipFileLocation = os.path.dirname(ZipFilePath)

    ZipFileContent = dict()
    ZipFileContentSize = 0
    with ZipFile(ZipFilePath, 'r') as ZipFileFolder:
        for name in ZipFileFolder.namelist():
            ZipFileContent[name] = ZipFileFolder.getinfo(name).file_size

        ZipFileContentSize = sum(ZipFileContent.values())
        ExtractedContentSize = 0
        startTime = time.time()
        for zippedFileName, zippedFileSize in ZipFileContent.items():
            UnzippedFilePath = os.path.abspath(f"{ZipFileLocation}/{zippedFileName}")
            os.makedirs(os.path.dirname(UnzippedFilePath), exist_ok=True)

            if os.path.isfile(UnzippedFilePath):
                ZipFileContentSize -= zippedFileSize
            else:
                ZipFileFolder.extract(zippedFileName, path=ZipFileLocation, pwd=None)
                ExtractedContentSize += zippedFileSize
            try:
                done = int(50*ExtractedContentSize/ZipFileContentSize)
                percentage = (ExtractedContentSize / ZipFileContentSize) * 100
            except ZeroDivisionError:
                done = 50
                percentage = 100
            elapsedTime = time.time() - startTime

            try:
                avgKBPerSecond = (ExtractedContentSize / 1024) / elapsedTime
            except ZeroDivisionError:
                avgKBPerSecond = 0.0

            avgSpeedString = '{:.2f} KB/s'.format(avgKBPerSecond)

            if (avgKBPerSecond > 1024):
                avgMBPerSecond = avgKBPerSecond / 1024
                avgSpeedString = '{:.2f} MB/s'.format(avgMBPerSecond)

            sys.stdout.write('\r[{}{}] {:.2f}% ({})     '.format('█' * done, '.' * (50-done), percentage, avgSpeedString))
            sys.stdout.flush()

    sys.stdout.write('\n')

    if DeleteZipFile:
        os.remove(ZipFilePath) # delete zip file

def PrintHeader(length, color, style=Style.BRIGHT):
    print(f"{style}{color}{'=' * length}{Style.RESET_ALL}")

def PrintBanner(text, width=50, color=Fore.CYAN, border_char='*'):
    # Center the text, padded to the specified width.
    border = border_char * width
    centered_text = text.center(width - 2)

    side_border = '|' if border_char in ['=', '-'] else border_char

    print(f"{Style.BRIGHT}{color}{border}")
    print(f"{side_border}{centered_text}{side_border}")
    print(f"{border}{Style.RESET_ALL}")
