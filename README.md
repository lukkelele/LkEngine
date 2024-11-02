# LkEngine :shaved_ice: 
> Game Engine written in C++20<br>
>
> Upgraded version of my first engine **LukkelEngine** ([repo](https://github.com/lukkelele/LukkelEngine/tree/main)).<br>
---

## Content
1. [About](#about)
2. [Setup and Installation](#setup-and-installation)
    - [Prerequisites](#prerequisites)
	- [Building LkEngine](#building-lkengine)
3. [Todo](#todo)
4. [Development](#development)
    - [LkEngine](#lkengine-development)
    - [LukkelEngine (old)](#lukkelengine-old)

---

# Project Information

LkEngine is a game engine written in C\++20. <br>
There was an earlier version called **LukkelEngine** that I scrapped and began anew.<br>
The sole reason for the project is to learn more and have fun. I really enjoy coding so I have a blast whilst doing it.<br>
I have lots of ideas that I want to see if they could work and this project has been a very good environment for me to try stuff out.
<br>
There is no roadmap, I just do things as they pop up in my head.<br>
<br>
My goal is to have a robust application that is _easy_ to extend and add features on.<br>
A rich core API that is good for performance and easy to use and grasp.<br>
That includes:
- Static reflection.
- Development tools _(e.g for debugging and profiling)_.
- Serialization, configurability and persistance.
- Dynamic configurability without rebuilding _(e.g changing log levels, enabling profiling)_.
- Crash handling.

<br>

This is somewhat of a playground for me. I will not (always) conform to standard practices as otherwise is a good rule to follow for a _real_ project.
Custom data structures will be used as a way for me to learn to first and foremost understand **_how they work_** and also **_why_** one should avoid doing so in the real world.
Implementing your own data structures is a great way to learn move semantics and copying semantics, and also see the potential pitfalls in both.

## Setup and Installation

### Building the project 
There are scripts available in `Tools/` to help with setting up the project.

1. Clone the repo and download all submodules.<br>
    ```shell
	git clone --recursive https://github.com/lukkelele/LkEngine.git
	```
2. Run the python script **_Setup.py_**.<br>
	```shell
	python Setup.py
    ```
3. Build the project by running **_Build.py_**.<br>
    ```shell
	python Build.py --build=[debug|release]
	```
    There are several build flags available:
	- --build
4. Run LkEngine. (FIXME: This will change)<br>
    ```shell
	python Run-LkEngine.py
	```

### Dependencies (TODO: Add reference links to the repo for every dependency)
- **glfw3**
- **glad**
- **spdlog**
- **ImGui**
- **ImGuizmo**
- **imgui-node-editor**
- **yaml-cpp**
- **glm**
- **entt**
- **Tracy**
- **assimp**
- **box2D**
- **python3** *(for project setup)*

## Features

### Available
TODO

### In Progress
:black_square_button: Camera System<br>
:black_square_button: Profiling and Instrumentation<br>
:black_square_button: Serialization<br>
:black_square_button: Physics and Raycasting<br>
:black_square_button: Asset Manager<br>
:black_square_button: Renderer Submission System<br>

## Todo
:black_square_button: Runtime application (instead of just sandbox)<br>
:black_square_button: Vulkan Renderer <br>
:black_square_button: Renderer debugging tools (e.g drawing lines in world space) <br>
:black_square_button: 3D Physics and Raycasting <br>

<!------------------------------------------------------------>

<br>
<br>
<br>

---

# Development ( MOVE TO SEPARATE FILE )

## Progression

### February 15, 2024
Last couple of weeks has mostly been work on assets and asset loading. <br>
Some nice improvements can be seen though, especially with the new environment cubemap.
<div align="center">
	<img align="center" src="/Docs/Images/LkEngine-2024-02-15_1.png" width=740 height=440>
	<p align="center">
</div>

<div align="center">
	<img align="center" src="/Docs/Images/LkEngine-2024-02-15_2.png" width=740 height=440>
	<p align="center">
	</p>
</div>

<div align="center">
	<img align="center" src="/Docs/Images/LkEngine-2024-02-15_3.png" width=740 height=440>
	<p align="center">
	</p>
</div>

<br>

### February 4, 2024
<div align="center">
	<img align="center" src="/Docs/Images/LkEngine-2024-02-04.png" width=740 height=440>
	<p align="center">
	Faulty renderered mesh because of a misaligned vertexbuffer and shader layout
	</p>
</div>

<br>

### January 28, 2024
Some 3D rendering, the **Translate** and **Rotate** gizmo modes can be seen as well.
<div align="center">
	<img align="center" src="/Docs/Images/LkEngine-2024-01-28_1.png" width=740 height=440>
	<p align="center"> </p>
</div>

<div align="center">
	<img align="center" src="/Docs/Images/LkEngine-2024-01-28_2.png" width=740 height=440>
	<p align="center">
	</p>
</div>

<br>

### January 1, 2024
The current look of the engine. Lots to polish.
<div align="center">
	<img align="center" src="/Docs/Images/LkEngine-2024-01-01.png" width=740 height=440>
	<p align="center">
	</p>
</div>

Once again thanks to Atilla for his handsome face. I need to add new images for 
the *Create Menu* (what I call it for now) but Atilla's beauty will do for now.

<br>

---


### December 4, 2023
It has been a while since I last posted some progress.<br>
What's new is that I've added functionality to pass textures to draw calls for 2D rendering 
which can be seen in the images below :smile:<br>
(sincere thanks to Atilla for being my debugging texture)
<div align="center">
	<img align="center" src="/Docs/Images/LkEngine-4-12-2023_1.png" width=740 height=440>
	<p align="center">
	<br>
	The different rendered entities can all be interacted with, the selected entity menu is shown to the right
	</p>
	<br>
	<img align="center" src="/Docs/Images/LkEngine-4-12-2023_2.png" width=740 height=440>
	<p align="center">
	</p>
</div>
<br>


### November 1, 2023

<div align="center">
	<img align="center" src="/Docs/Images/LkEngine-1-11-2023.png" width=740 height=440>
	<p align="center">
		Rendering a grid, will eventually expand to be used as a base for creating 2D games such as Super Mario
	</p>
</div>
<br>


### October 29, 2023

<div align="center">
	<img align="center" src="/Docs/Images/LkEngine-29-10-2023.png" width=740 height=440>
	<p align="center">
		Rendering lines and quads using the new 2D renderer
	</p>
</div>
<br>

### October 20, 2023

<div align="center">
	<img align="center" src="/Docs/Images/LkEngine-20-10-2023.png" width=740 height=440>
<p align="center">
Rendered entities that consist of mesh, sprite and transform components. The UI has been improved.
</p>

</div>
<br>

### October 16, 2023

<div align="center">
	<img align="center" src="/Docs/Images/LkEngine-16-10-2023_2.png" width=740 height=440>
<p align="center">
Rendering multiple rectangles that can be adjusted indivually thanks to the entity component system.
The entity entries can be seen on the right sidebar.
</p>

</div>
<br>


<div align="center">
	<img align="center" src="/Docs/Images/LkEngine-16-10-2023_1.png" width=740 height=440>
<p align="center">
Got a base UI layout going with two rectangles that can be moved and have their colors changed.
</p>
</div>


<br>

## LukkelEngine (old) 

### March 19, 2023
Improved the UI, added constraints and updated the event handling. I was in dire need to use an event system as the 
world physics cant really function without it. 

<div align="center">
	<img align="center" src="/Docs/Images/old/lukkelengine-03-19-3.png" width=740 height=440>
<p align="center">The cyan cube has a pivot constraint that can be added and removed with the UI (constraints menu to the left)</p>
</div>

<div align="center">
	<img align="center" src="/Docs/Images/old/lukkelengine-03-19-2.png" width=740 height=440>
<p align="center"></p>
</div>

<div align="center">
	<img align="center" src="/Docs/Images/old/lukkelengine-03-19-1.png" width=740 height=440>
<p align="center">Colliding objects are set to the color black whenever a valid detection takes place</p>
</div>

### March 12, 2023

<div align="center">
	<img align="center" src="/Docs/Images/old/lukkelengine-03-12-1.png" width=740 height=440>
<p align="center"></p>
</div>

<div align="center">
	<img align="center" src="/Docs/Images/old/lukkelengine-03-12-2bodyPicked.png" width=740 height=440>
<p align="center">Selected object with raycasting after clicking left mouse button</p>
</div>

<div align="center">
	<img align="center" src="/Docs/Images/old/lukkelengine-03-12-3-changecolors.png" width=740 height=440>
<p align="center">Changed color of selected object</p>
</div>

<div align="center">
	<img align="center" src="/Docs/Images/old/lukkelengine-03-12-4.png" width=740 height=440>
<p align="center">View of the platform</p>
</div>

---

### February 26, 2023

<div align="center">
	<img align="center" src="/Docs/Images/old/engine-26-2-2023-pic1.png" width=740 height=440>
</div>
<p align="center">Scene overview with a cube</p>

<br>
<div align="center">
	<img align="center" src="/Docs/Images/old/engine-26-2-2023-pic2.png" width=740 height=440>
<p align="center">The same cube but from a closer distance</p>
</div>

<br>


--- 


