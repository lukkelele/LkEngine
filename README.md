# LkEngine :shaved_ice: 
> Game Engine written in C++20<br>

---

<img height="22" alt="VersionBadge" src="https://img.shields.io/badge/LkEngine-v0.1.2--alpha-blue">

![Build-Engine](https://github.com/lukkelele/LkEngine/actions/workflows/Build-Engine.yml/badge.svg)
<img height="20" alt="Endpoint Badge" src="https://img.shields.io/endpoint?url=https%3A%2F%2Flukkelele.github.io%2FLkEngine%2FTestResults%2FCoreTests%2FCoreTests-Badge.json&style=plastic&label=Core-Tests">

## Platform Support
#### :white_check_mark: Windows<br>
#### :black_square_button: Linux <br>

## Content
1. [Project](#Project)
2. [Setup and Installation](#Setup-and-Installation)
    - [Windows](#Platform-Windows)
	- [Linux](#Platform-Linux)
	- [Dependencies](#Engine-Dependencies)
3. [Engine Features](#Engine-Features)
4. [Development](#Engine-Development)

---

# Project <a id="Project"></a> <a href="https://lukkelele.github.io/LkEngine" target="_blank"> <img height="23" alt="github.io.LkEngine" src="https://img.shields.io/badge/github.io-LkEngine-darkgray">
</a>

**LkEngine** (pronounced *Lukkel-Engine*) is a game engine written in C\++20. <br>
The sole reason for the project is to learn more and have fun. I really enjoy coding so I have a blast doing it.<br>
I have lots of ideas that I want to see if they could work and this project has been a very good environment for me to try stuff out.
<br>
<br>
The goal is to have an application with an API that is good for performance and easy to use.<br>
With features such as:
- Multiple platform support.
- Static reflection.
- Serialization and persistance.
- Development tools (e.g debugging and profiling).
- Crash and error handling.
- Application launcher.

## Setup and Installation <a id="Setup-and-Installation"></a>
For development I've been using Visual Studio Community 22 (64-bit).<br>
Other IDE's should in theory be able to build and run the engine but I cannot make any promises.

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

## Engine Features <a id="Engine-Features"></a>

### Available
:white_check_mark: Serialization<br>
:white_check_mark: Camera System<br>

### In Progress
:black_square_button: Static Reflection<br>
:black_square_button: Profiling and Instrumentation<br>
:black_square_button: Physics and Raycasting<br>
:black_square_button: Asset Manager<br>
:black_square_button: Renderer Submission System<br>

---

### Todo
:small_blue_diamond: **Vulkan Support.**<br>
:small_blue_diamond: **Renderer Debugging tools (e.g drawing lines in world space).**<br>
:small_blue_diamond: **3D Physics and Raycasting.**<br>

<!------------------------------------------------------------>

---

## Misc


* [github.io.lukkelele/LkEngine](https://lukkelele.github.io/LkEngine/) <br>
* [github.io.lukkelele/LkEngine/Development](https://github.com/lukkelele/LkEngine/blob/gh-pages/Development.md)

### :heavy_minus_sign: [Development](Docs/Development.md) <a id="Engine-Development"></a>
