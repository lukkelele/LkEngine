# Building LkEngine
> Setup and installation steps for the engine.

The engine has been built and verified on the following compilers.  
Other versions _might_ work as well.

| **Compiler** | **Version** | **Supported** |
| :-------- | :-------: | :------: |
| MSVC | 17.9.7 (1939)| :white_check_mark: |
| GCC | 14.2.1 | :white_check_mark: |
| Clang | - | :x: |

## Content
1. [Prerequities](#Prerequities)
2. [Setup](#Setup)
3. [Build](#Build) 
    * [Build Configurations](#Build-Configurations)
    * [Project Generation](#Project-Generation)
    * [External Libraries](#External-Libraries)

---

## Prerequities <a id="Prerequities"></a>

The engine depends on Git and Python 3 for the initial setup steps.

| Package | Link                          |
|----------------|--------------------------------------------|
| Git | [Download](https://git-scm.com/downloads)
| Python 3 | [Download](https://www.python.org/downloads/windows/) |

### Windows <a id="Platform-Windows"></a>
> Note: There are .bat scripts in [Tools](./Tools) to help with setting up the engine without the need to use a shell.

Make sure the following are installed on Windows:

| Package | Link                          |
|----------------|--------------------------------------------|
| C/C++ Compiler | [Download](https://visualstudio.microsoft.com/vs/features/cplusplus/) |
| CMake | [Download](https://cmake.org/download/) |
| Make | [Download](https://gnuwin32.sourceforge.net/packages/make.htm) |

### Linux <a id="Platform-Linux"></a>
> Make use of the distro package manager to install the dependencies.  
> The following steps use **apt** from Ubuntu.

A supported compiler version is required to be installed as well.

```shell
sudo apt install build-essential git xorg libgtk-3-dev cmake python3
```

---

## Setup <a id="Setup"></a>

1. Clone the repo and with all submodules.<br>
    ```shell
	git clone --recursive https://github.com/lukkelele/LkEngine.git
	```
2. Run the setup script **Setup.py**.
	```shell
	python Setup.py
    ```
3. All dependencies are handled by the script. The built libraries are moved to _External/Libraries_.
4. Done! :white_check_mark:

---

## Build <a id="Build"></a>

The engine can be built with an IDE or by running the build script.  

```shell
python Build.py [-c|--config] 
				[-g|--generate-projects] 
				[--build] 
				[--clean] 
				[--purge] 
				[--build-assimp] 
				[--help] 
```

### Build <a id="Build-Configurations"></a>

```shell
# Config: Debug
python Build.py --config Debug

# Config: Release
python Build.py --config Release

# Config: Dist
python Build.py --config Dist

# Config: Debug-AS (Address Sanitize)
python Build.py --config Debug-AS
```

### Project Generation <a id="Project-Generation"></a>
```shell
# Generate project files.
python Build.py --generate-projects

# Generate project files and start build immediately after.
python Build.py --generate-projects --build
```

### External Libraries <a id="External-Libraries"></a>

Most libraries are configured and built with premake but a few are built using **Build.py**.

```shell
# Build Assimp.
python Build.py --build-assimp
```
