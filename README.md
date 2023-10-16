# LkEngine :shaved_ice: 
> Graphics Engine written in C++17<br>
>
> New version of my first engine **LukkelEngine** ([repo](https://github.com/lukkelele/LukkelEngine/tree/main)).<br>
---

## Content
1. [About](#about)
2. [Setup and Installation](#setup-and-installation)
    - [Prerequisites](#prerequisites)
	- [Building LkEngine](#building-lkengine)
3. [Todo](#todo)
4. [Implementation](#implementation)
5. [Development](#development)
    - [LkEngine](#LkEngine-dev)
    - [LukkelEngine (old)](#LukkelEngine-dev)

## About 

When I began my first engine **LukkelEngine** I had lots to learn about graphics and engines (still do).
Because of this, lots of focus was aimed towards things such as 3D objects and rigid bodies.<br>
This caused some flaws in the base design that I wasn't too happy with.<br>
<br>
That is why **LkEngine** will focus towards 2D in the beginning and when I feel as if the engine is ready I 
will begin to implement 3D features.
This is done to create a better base to expand on in the future.<br>
<br>
When the 2D implementation is done, the plan is to create a game such as **Super Mario** or **Pokémon** 
(similar to a 'classical' Gameboy game).

## Setup and Installation

### Prerequisites
- glfw3
- Vulkan
- OpenGL4
- spdlog
- ImGui
- ImGuizmo

### Building LkEngine
I use premake5 because it is able to create solution files in a neat way.
This allows for (almost) identical development on both Windows and Linux.
CMake is a choice as well, there are CMakeLists.txt available but there is some configuration to do.
<br>
<br>
Use the build script in the repo or execute premake5 manually in bash (or PS). 
<br>
Manually in Windows:
```
 lib\premake-5.0\premake5.exe vs2022 
```
And on Linux:
```
mkdir build
cd build
../lib/premake-5.0/premake5 gmake
make
```

---

## Todo

<!---------------------- Checklist --------------------------->
### General
:black_square_button: Raycasting <br>
:black_square_button: Shader hot reloading<br>
:black_square_button: Collision system<br>

### 2D 
:black_square_button: Ortographic camera<br>
:black_square_button: Sprite rendering<br>

### 3D
:black_square_button: Physics <br>
:black_square_button: Rigidbody constraints <br>

<!------------------------------------------------------------>


---

## Implementation


### Application

### Renderer

### Layer system

---

## Development

## LkEngine {#LkEngine-dev}
### October 16, 2023
Got a base UI layout going with two rectangles that can be moved and have their colors changed.
<div align="center">
	<img align="center" src="/doc/img/LkEngine-16-10-2023_1.png" width=740 height=440>
<p align="center">
</p>
</div>


<br>

## LukkelEngine (old) {#LukkelEngine-dev}

### March 19, 2023
Improved the UI, added constraints and updated the event handling. I was in dire need to use an event system as the 
world physics cant really function without it. 

<div align="center">
	<img align="center" src="/doc/img/old/lukkelengine-03-19-3.png" width=740 height=440>
<p align="center">The cyan cube has a pivot constraint that can be added and removed with the UI (constraints menu to the left)</p>
</div>

<div align="center">
	<img align="center" src="/doc/img/old/lukkelengine-03-19-2.png" width=740 height=440>
<p align="center"></p>
</div>

<div align="center">
	<img align="center" src="/doc/img/old/lukkelengine-03-19-1.png" width=740 height=440>
<p align="center">Colliding objects are set to the color black whenever a valid detection takes place</p>
</div>

### March 12, 2023

<div align="center">
	<img align="center" src="/doc/img/old/lukkelengine-03-12-1.png" width=740 height=440>
<p align="center"></p>
</div>

<div align="center">
	<img align="center" src="/doc/img/old/lukkelengine-03-12-2bodyPicked.png" width=740 height=440>
<p align="center">Selected object with raycasting after clicking left mouse button</p>
</div>

<div align="center">
	<img align="center" src="/doc/img/old/lukkelengine-03-12-3-changecolors.png" width=740 height=440>
<p align="center">Changed color of selected object</p>
</div>

<div align="center">
	<img align="center" src="/doc/img/old/lukkelengine-03-12-4.png" width=740 height=440>
<p align="center">View of the platform</p>
</div>

---

### February 26, 2023

<div align="center">
	<img align="center" src="/doc/img/old/engine-26-2-2023-pic1.png" width=740 height=440>
</div>
<p align="center">Scene overview with a cube</p>

<br>
<div align="center">
	<img align="center" src="/doc/img/old/engine-26-2-2023-pic2.png" width=740 height=440>
<p align="center">The same cube but from a closer distance</p>
</div>

<br>


--- 


