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
4. [Development](#development)
    - [LkEngine](#lkengine-development)
    - [LukkelEngine (old)](#lukkelengine-old)

## About 

When I began my first engine **LukkelEngine** I had lots to learn about graphics and engines (still do),
and because of this, lots of focus was aimed towards things such as 3D objects and rigid bodies.<br>
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
TODO: *this section is to be written in the **somewhat near** future*
<br>

## Todo
<!---------------------- Checklist --------------------------->

### 2D 
:white_check_mark: Ortographic camera<br>
:white_check_mark: Sprite rendering<br>
:black_square_button: Collision system<br>
:black_square_button: Grid creation<br>
:white_check_mark: Raycasting <br>
:black_square_button: Physics <br>

### 3D
:black_square_button: Raycasting <br>
:black_square_button: Physics <br>
:black_square_button: Rigidbody constraints <br>

<!------------------------------------------------------------>


---

# Development

## LkEngine Development

### January 1, 2024
The current look of the engine. Lots to polish. The creation of scene entities through the UI is what's focused for now. 
Which means that the UI is what I spend the most time on.
<div align="center">
	<img align="center" src="/doc/img/LkEngine-2024-01-01.png" width=740 height=440>
	<p align="center">
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
	<img align="center" src="/doc/img/LkEngine-4-12-2023_1.png" width=740 height=440>
	<p align="center">
	<br>
	The different rendered entities can all be interacted with, the selected entity menu is shown to the right
	</p>
	<br>
	<img align="center" src="/doc/img/LkEngine-4-12-2023_2.png" width=740 height=440>
	<p align="center">
	</p>
</div>
<br>


### November 1, 2023

<div align="center">
	<img align="center" src="/doc/img/LkEngine-1-11-2023.png" width=740 height=440>
	<p align="center">
		Rendering a grid, will eventually expand to be used as a base for creating 2D games such as Super Mario
	</p>
</div>
<br>


### October 29, 2023

<div align="center">
	<img align="center" src="/doc/img/LkEngine-29-10-2023.png" width=740 height=440>
	<p align="center">
		Rendering lines and quads using the new 2D renderer
	</p>
</div>
<br>

### October 20, 2023

<div align="center">
	<img align="center" src="/doc/img/LkEngine-20-10-2023.png" width=740 height=440>
<p align="center">
Rendered entities that consist of mesh, sprite and transform components. The UI has been improved.
</p>

</div>
<br>

### October 16, 2023

<div align="center">
	<img align="center" src="/doc/img/LkEngine-16-10-2023_2.png" width=740 height=440>
<p align="center">
Rendering multiple rectangles that can be adjusted indivually thanks to the entity component system.
The entity entries can be seen on the right sidebar.
</p>

</div>
<br>


<div align="center">
	<img align="center" src="/doc/img/LkEngine-16-10-2023_1.png" width=740 height=440>
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


