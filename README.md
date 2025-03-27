# LkEngine

<!-- Badges -->
<img height="20" alt="VersionBadge" src="https://img.shields.io/badge/Version-0.1.2--alpha-blue"></img>
![Build-Windows](https://github.com/lukkelele/LkEngine/actions/workflows/Build-Windows.yml/badge.svg)
![Build-Linux](https://github.com/lukkelele/LkEngine/actions/workflows/Build-Linux.yml/badge.svg)
<img height="20" alt="Endpoint Badge" src="https://img.shields.io/endpoint?url=https%3A%2F%2Flukkelele.github.io%2FLkEngine%2FTestResults%2FCore%2FBadge.json&style=plastic&label=Core-Tests"></img>

<p align="center">
	<img src="./Docs/Images/Banner/LkEngine.png" />
</p>

**LkEngine** (pronounced *Lukkel-Engine*) is a game and rendering engine written in C\++20. <br>
This project is for fun and to learn more, I just love to code.<br>
I have lots of ideas that I want to see if they could work and this project is good environment for me to try stuff out.

## Content
1. [Engine](#Engine)
2. [Setup and Build](#Setup-and-Build)
3. [Development](#Engine-Development)
	- [Showcase](#Engine-Development-Showcase)
4. [Dependencies](#Engine-Dependencies)

---

* [Engine Design](./Docs/Engine.md) <br>
* [Engine API](https://lukkelele.github.io/LkEngine/Doxygen/Generated/Html/index.html) <br>
* [Development Journal](./Docs/Development.md) <br>
* [github.io.lukkelele/LkEngine](https://lukkelele.github.io/LkEngine/) <br>

---

## Engine <a id="Engine"></a>
For a detailed view of the engine, check out the [Engine Design](./Docs/Engine.md) 
and the [Engine API](https://lukkelele.github.io/LkEngine/Doxygen/Generated/Html/index.html).

#### Implemented
:white_check_mark: OpenGL support (4.5, 3.3)<br>
:white_check_mark: Profiling and instrumentation<br>
:white_check_mark: Serialization and persistence<br>
:white_check_mark: Camera system<br>
:white_check_mark: Raycasting<br>
:white_check_mark: Asset loading (gltf, fbx, obj)<br>
:white_check_mark: Custom themes<br>
:white_check_mark: Automated tests<br>
:white_check_mark: CI/CD<br>

#### In Progress
:large_orange_diamond: Editor (LkEditor)<br>
:large_orange_diamond: Test Framework (LTesto)<br>
:large_orange_diamond: Rendering debug tools (e.g drawing lines in world space)<br>

#### Todo
:black_square_button: Static reflection<br>
:black_square_button: Physics (2D/3D) <br>
:black_square_button: Vulkan support<br>

---

## Setup and Build <a id="Setup-and-Build"></a>

View [BUILD.md](./BUILD.md) for information on how to setup and build the engine.

| **Compiler** | **Version** | **Supported** |
| :-------- | :-------: | :------: |
| MSVC | 17.9.7 (1939)| :white_check_mark: |
| GCC | 14.2.1 | :white_check_mark: |
| Clang | - | :x: |

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
