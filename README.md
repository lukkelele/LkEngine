# LkEngine :shaved_ice: 
> Game Engine written in C++20<br>

---

![Build-Engine](https://github.com/lukkelele/LkEngine/actions/workflows/Build-Engine.yml/badge.svg)
![Core-Tests](https://github.com/lukkelele/LkEngine/actions/workflows/Core-Tests.yml/badge.svg)


## Platform Support
#### :white_check_mark: Windows<br>
#### :black_square_button: Linux <br>


## Content
1. [Project](#project)
2. [Setup and Installation](#Setup-and-Installation)
    - [Windows](#Platform-Windows)
	- [Linux](#Platform-Linux)
	- [Dependencies](#Engine-Dependencies)
3. [Engine Features](#Engine-Features)
4. [Development](#development)
    - [LkEngine](#lkengine-development)
    - [LukkelEngine (old)](#lukkelengine-old)

---

# Project <a id="Project"></a>

**LkEngine** (pronounced *_Lukkel-Engine_*) is a game engine written in C\++20. <br>
The sole reason for the project is to learn more and have fun. I really enjoy coding so I have a blast doing it.<br>
I have lots of ideas that I want to see if they could work and this project has been a very good environment for me to try stuff out.
<br>
<br>
My goal is to have a robust application that is _easy_ to extend and add features on.<br>
A rich core API that is good for performance and easy to use.<br>
With features such as:
- Static reflection.
- Development tools _(e.g for debugging and profiling)_.
- Serialization, configurability and persistance.
- Dynamic configurability without rebuilding _(e.g changing log levels, enabling profiling)_.
- Crash and error handling.
- Multiple platform support.

This is somewhat of a playground for me. I will not always conform to standard practices as otherwise is a good rule to follow for a "_real_" project.

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

## Engine Features

### Available
:white_check_mark:

### In Progress
:black_square_button: Static Reflection<br>
:black_square_button: Camera System<br>
:black_square_button: Profiling and Instrumentation<br>
:black_square_button: Serialization<br>
:black_square_button: Physics and Raycasting<br>
:black_square_button: Asset Manager<br>
:black_square_button: Renderer Submission System<br>

---

## Todo
:small_blue_diamond: **Vulkan Support.**<br>
:small_blue_diamond: **Renderer Debugging tools (e.g drawing lines in world space).**<br>
:small_blue_diamond: **3D Physics and Raycasting.**<br>

<!------------------------------------------------------------>

---

## Misc

:warning: TODO: Move this to the github.io page instead.

### :heavy_minus_sign: [Development](Docs/Development.md)
