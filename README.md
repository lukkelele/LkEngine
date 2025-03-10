# LkEngine :shaved_ice: 

<!-- Badges -->
<img height="20" alt="VersionBadge" src="https://img.shields.io/badge/Version-0.1.2--alpha-blue"></img>
![Build-Engine](https://github.com/lukkelele/LkEngine/actions/workflows/Build-Engine.yml/badge.svg)
<img height="20" alt="Endpoint Badge" src="https://img.shields.io/endpoint?url=https%3A%2F%2Flukkelele.github.io%2FLkEngine%2FTestResults%2FCore%2FBadge.json&style=plastic&label=Core-Tests"></img>

<p align="center">
	<img src="./Docs/Images/Banner/LkEngine.png" />
</p>

## Content
1. [Project](#Project)
2. [Engine](#Engine)
3. [Setup and Installation](#Setup-and-Installation)
    - [Windows](#Platform-Windows)
	- [Linux](#Platform-Linux)
4. [Development](#Engine-Development)
	- [Showcase](#Engine-Development-Showcase)
5. [Dependencies](#Engine-Dependencies)

---

* [Engine API](./Docs/Engine.md) <br>
* [Development Journal](./Docs/Development.md) <br>
* [github.io.lukkelele/LkEngine](https://lukkelele.github.io/LkEngine/) <br>
* [github.io.lukkelele/LkEngine/Development](https://github.com/lukkelele/LkEngine/blob/gh-pages/Development.md)

---

# Project <a id="Project"></a> 

**LkEngine** (pronounced *Lukkel-Engine*) is a game and rendering engine written in C\++20. <br>
This project is for fun and to learn more, I just love to code.<br>
I have lots of ideas that I want to see if they could work and this project is good environment for me to try stuff out.

## Engine <a id="Engine"></a>
For a detailed view of the engine and the API, check out the [Engine API](./Docs/Engine.md).

#### Implemented
:white_check_mark: OpenGL support<br>
:white_check_mark: Asset loading<br>
:white_check_mark: Profiling and instrumentation<br>
:white_check_mark: Serialization and persistence<br>
:white_check_mark: Automated tests<br>
:white_check_mark: Camera system<br>
:white_check_mark: Raycasting<br>
:white_check_mark: Custom themes<br>

#### In Progress
:large_orange_diamond: Editor (LkEditor)<br>
:large_orange_diamond: Test Framework (LTesto)<br>
:large_orange_diamond: Rendering debug tools (e.g drawing lines in world space)<br>

#### Todo
:black_square_button: Static reflection<br>
:black_square_button: Physics (2D/3D) <br>
:black_square_button: Vulkan support<br>

---

## Setup and Installation <a id="Setup-and-Installation"></a>

| **Platform** | **Support** |
| :-------- | :-------: |
| Windows  | :white_check_mark: |
| Linux | :x: |

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
> :warning: Linux support is coming in the future.

---

## Development <a id="Engine-Development"></a>

For development I've been using Visual Studio Community 22 (64-bit) on Windows.<br>

See more images and screenshots in the [development](Docs/Development.md) documentation.

### Showcase <a id="Engine-Development-Showcase"></a>

![2025-February-02-1](Docs/Images/2025/LkEngine-2025-02-02_1.png)
![2025-February-02-2](Docs/Images/2025/LkEngine-2025-02-02_2.png)
![2025-February-02-3](Docs/Images/2025/LkEngine-2025-02-02_3.png)
![2025-February-02-3](Docs/Images/2025/LkEngine-2025-02-02_4.png)

![2025-January-26-1](Docs/Images/2025/LkEngine-2025-01-26_1.png)
![2025-January-26-2](Docs/Images/2025/LkEngine-2025-01-26_2.png)
![2025-January-26-3](Docs/Images/2025/LkEngine-2025-01-26_3.png)

---

## Dependencies <a id="Engine-Dependencies"></a>
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
