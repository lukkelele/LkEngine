# LkEngine :shaved_ice: 
> Game Engine written in C++20

---

<img height="22" alt="VersionBadge" src="https://img.shields.io/badge/LkEngine-v0.1.2--alpha-blue">

![Build-Engine](https://github.com/lukkelele/LkEngine/actions/workflows/Build-Engine.yml/badge.svg)
<img height="20" alt="Endpoint Badge" src="https://img.shields.io/endpoint?url=https%3A%2F%2Flukkelele.github.io%2FLkEngine%2FTestResults%2FCore%2FBadge.json&style=plastic&label=Core-Tests">

## Content
1. [Project](#Project)
2. [Setup and Installation](#Setup-and-Installation)
    - [Windows](#Platform-Windows)
	- [Linux](#Platform-Linux)
3. [Engine](#Engine)
	- [Features](#Engine-Features)
	- [Dependencies](#Engine-Dependencies)
4. [Development](#Engine-Development)
	- [Showcase](#Engine-Development-Showcase)

---

* [github.io.lukkelele/LkEngine](https://lukkelele.github.io/LkEngine/) <br>
* [github.io.lukkelele/LkEngine/Development](https://github.com/lukkelele/LkEngine/blob/gh-pages/Development.md)

---

# Project <a id="Project"></a> <a href="https://lukkelele.github.io/LkEngine" target="_blank"> <img height="23" alt="github.io.LkEngine" src="https://img.shields.io/badge/github.io-LkEngine-darkgray">
</a>

**LkEngine** (pronounced *Lukkel-Engine*) is a game engine written in C\++20. <br>
This project is for fun and to learn more. I really enjoy coding so I have a blast doing it.<br>
I have lots of ideas that I want to see if they could work and this project is good environment for me to try stuff out.
<br>
<br>
The goal is to have a rendering application with an API that is good for performance and easy to use.<br>
With features such as:
- Static reflection
- Support for multiple platforms
- Crash and error handling
- Automated testing
- Serialization and persistance
- Application launcher
- Development tools (e.g profiling, renderer debugging)

## Setup and Installation <a id="Setup-and-Installation"></a>
For development I've been using Visual Studio Community 22 (64-bit) on Windows.<br>

### Windows <a id="Platform-Windows"></a>
Make sure the following are installed:

| Package | Link                          |
|----------------|--------------------------------------------|
| CMake | [Download](https://cmake.org/download/) |
| C/C++ Compiler | [Download](https://visualstudio.microsoft.com/vs/features/cplusplus/) |
| Make | [Download](https://gnuwin32.sourceforge.net/packages/make.htm) |
| Python3 | [Download](https://www.python.org/downloads/windows/) |

Most dependencies are sorted by scripts available in `Tools/` that help setting up the engine.


1. Clone the repo and with all submodules.<br>
    ```shell
	git clone --recursive https://github.com/lukkelele/LkEngine.git
	```
2. Enter the **Tools** directory.<br>
3. Run the setup script **LkEngine-Setup.bat**.<br>
   It is also possible to run the script directly in a shell which yields the same result as the bat script runs **LkEngine-Setup.py**.
	```shell
	python LkEngine-Setup.py
    ```
4. All dependencies are then handled by the script. The built static libraries are moved to their desired spot inside the _'External'_ directory. 
5. Done! :white_check_mark:

### Linux <a id="Platform-Linux"></a>
:warning: **TODO**

---

# Engine <a id="Engine"></a>

| **Platform** | **Support** |
| :-------- | :-------: |
| Windows  | :white_check_mark: |
| Linux | :x: |

The documentation for the LkEngine API can be found in [Engine.md](./Docs/Engine.md) in the [documentation](./Docs) folder.

## Features <a id="Engine-Features"></a>
#### Available
:white_check_mark: OpenGL Support<br>
:white_check_mark: Camera System<br>
:white_check_mark: Asset Loading<br>
:white_check_mark: Profiling and Instrumentation<br>
:white_check_mark: Serialization and Persistence<br>

#### In Progress
:large_orange_diamond: Editor (LkEditor)<br>
:large_orange_diamond: Test Framework (LTesto)<br>

#### Todo
:black_square_button: Static Reflection<br>
:black_square_button: Physics (2D/3D) <br>
:black_square_button: Debugging tools (e.g drawing lines in world space)<br>
:black_square_button: Vulkan support<br>

<!------------------------------------------------------------>

### Dependencies <a id="Engine-Dependencies"></a>
- [glfw3](https://github.com/glfw/glfw)
- [glad](https://github.com/Dav1dde/glad)
- [spdlog](https://github.com/gabime/spdlog)
- [ImGui](https://github.com/ocornut/imgui)
- [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)
- [imgui-node-editor](https://github.com/thedmd/imgui-node-editor)
- [nativefiledialog-extended](https://github.com/btzy/nativefiledialog-extended)
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)
- [glm](https://github.com/g-truc/glm)
- [entt](https://github.com/skypjack/entt)
- [Tracy](https://github.com/wolfpld/tracy)
- [assimp](https://github.com/assimp/assimp)
- [box2d](https://github.com/erincatto/box2d)
- [python3](https://www.python.org/downloads/)

---

## Development <a id="Engine-Development"></a>

See more images and screenshots in the [documentation](Docs/Development.md).

* [github.io.lukkelele/LkEngine](https://lukkelele.github.io/LkEngine/) <br>
* [github.io.lukkelele/LkEngine/Development](https://github.com/lukkelele/LkEngine/blob/gh-pages/Development.md)


### Showcase <a id="Engine-Development-Showcase"></a>

![2025-February-26-1](Docs/Images/2025/LkEngine-2025-02-02_1.png)
![2025-February-26-2](Docs/Images/2025/LkEngine-2025-02-02_2.png)
![2025-February-26-3](Docs/Images/2025/LkEngine-2025-02-02_3.png)
![2025-February-26-3](Docs/Images/2025/LkEngine-2025-02-02_4.png)

![2025-January-26-1](Docs/Images/2025/LkEngine-2025-01-26_1.png)
![2025-January-26-2](Docs/Images/2025/LkEngine-2025-01-26_2.png)
![2025-January-26-3](Docs/Images/2025/LkEngine-2025-01-26_3.png)
