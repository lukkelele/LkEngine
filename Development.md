# LkEngine Development
> Development progress.

The content is listed in a format of one yar at a time.

---

* [2024](#Year-2024)
  - [January](#2024-January)
  - [February](#2024-February)
* [2023](#2023)
  - [March](#2023-March)
  - [October](#2023-October)
  - [November](#2023-November)
  - [December](#2023-December)

---

# 2024 <a id="Year-2024"></a>
## January (2024)<a id="2024-January"></a>

### January 1
The current look of the engine, lots to polish.
<div align="center">
	<img align="center" src="Images/LkEngine-2024-01-01.png" width=740 height=440>
	<p align="center">
	</p>
</div>

Once again thanks to Atilla for his handsome face. I need to add new images for 
the *Create Menu* (what I call it for now) but Atilla's beauty will do for now.
<br>

### January 28
Some 3D rendering. The *Translate* and *Rotate* gizmo modes can be seen as well.
<div align="center">
	<img align="center" src="Images/LkEngine-2024-01-28_1.png" width=740 height=440>
	<p align="center"> </p>
</div>

<div align="center">
	<img align="center" src="Images/LkEngine-2024-01-28_2.png" width=740 height=440>
	<p align="center">
	</p>
</div>

---
<!----------------------------------------------------------->
<!--| FEBRUARY |-->
<!----------------------------------------------------------->

## February (2024)<a id="2024-February"></a>
### February 15
Last couple of weeks has mostly been work on assets and asset loading. <br>
Some nice improvements can be seen though, especially with the new environment cubemap.
<div align="center">
	<img align="center" src="Images/LkEngine-2024-02-15_1.png" width=740 height=440>
	<p align="center">
</div>

<div align="center">
	<img align="center" src="Images/LkEngine-2024-02-15_2.png" width=740 height=440>
	<p align="center">
	</p>
</div>

<div align="center">
	<img align="center" src="Images/LkEngine-2024-02-15_3.png" width=740 height=440>
	<p align="center">
	</p>
</div>

<br>

### February 4
<div align="center">
	<img align="center" src="Images/LkEngine-2024-02-04.png" width=740 height=440>
	<p align="center">
	Faulty renderered mesh because of a misaligned vertexbuffer and shader layout
	</p>
</div>

<br>

---
<!----------------------------------------------------------->
<!--| 2023 | DECEMBER |-->
<!----------------------------------------------------------->

# 2023
## December (2023)<a id="2023-December"></a>
### December 4
It has been a while since I last posted some progress.<br>
What's new is that I've added functionality to pass textures to draw calls for 2D rendering 
which can be seen in the images below :smile:.<br>
(sincere thanks to Atilla for being my debugging texture)
<div align="center">
	<img align="center" src="Images/LkEngine-4-12-2023_1.png" width=740 height=440>
	<p align="center">
	<br>
	The different rendered entities can all be interacted with, the selected entity menu is shown to the right
	</p>
	<br>
	<img align="center" src="Images/LkEngine-4-12-2023_2.png" width=740 height=440>
	<p align="center">
	</p>
</div>
<br>

---
<!----------------------------------------------------------->
<!--| 2023 | NOVEMBER |-->
<!----------------------------------------------------------->

## November (2023) <a id="2023-November"></a>
### November 1

<div align="center">
	<img align="center" src="Images/LkEngine-1-11-2023.png" width=740 height=440>
	<p align="center">
		Rendering a grid, will eventually expand to be used as a base for creating 2D games such as Super Mario
	</p>
</div>
<br>

<!----------------------------------------------------------->
<!--| 2023 | OCTOBER |-->
<!----------------------------------------------------------->

## October (2023) <a id="2023-October"></a>

### October 16

<div align="center">
	<img align="center" src="Images/LkEngine-16-10-2023_2.png" width=740 height=440>
<p align="center">
Rendering multiple rectangles that can be adjusted indivually thanks to the entity component system.
The entity entries can be seen on the right sidebar.
</p>

</div>
<br>

### October 20

<div align="center">
	<img align="center" src="Images/LkEngine-20-10-2023.png" width=740 height=440>
<p align="center">
Rendered entities that consist of mesh, sprite and transform components. The UI has been improved.
</p>

</div>
<br>

### October 29

<div align="center">
	<img align="center" src="Images/LkEngine-29-10-2023.png" width=740 height=440>
	<p align="center">
		Rendering lines and quads using the new 2D renderer
	</p>
</div>
<br>

<div align="center">
	<img align="center" src="Images/LkEngine-16-10-2023_1.png" width=740 height=440>
<p align="center">
Got a base UI layout going with two rectangles that can be moved and have their colors changed.
</p>
</div>

<br>

---

<br>

<!----------------------------------------------------------->
<!--| LukkelEngine OLD |-->
<!----------------------------------------------------------->

# LukkelEngine  (Old) <a id="LukkelEngine-Old"></a>
## March 2023
### March 19
Improved the UI, added constraints and updated the event handling. I was in dire need to use an event system as the 
world physics cant really function without it. 

<div align="center">
	<img align="center" src="Images/Old/lukkelengine-03-19-3.png" width=740 height=440>
<p align="center">The cyan cube has a pivot constraint that can be added and removed with the UI (constraints menu to the left)</p>
</div>

<div align="center">
	<img align="center" src="Images/Old/lukkelengine-03-19-2.png" width=740 height=440>
<p align="center"></p>
</div>

<div align="center">
	<img align="center" src="Images/Old/lukkelengine-03-19-1.png" width=740 height=440>
<p align="center">Colliding objects are set to the color black whenever a valid detection takes place</p>
</div>

<br>

### March 12

<div align="center">
	<img align="center" src="Images/Old/lukkelengine-03-12-1.png" width=740 height=440>
<p align="center"></p>
</div>

<div align="center">
	<img align="center" src="Images/Old/lukkelengine-03-12-2bodyPicked.png" width=740 height=440>
<p align="center">Selected object with raycasting after clicking left mouse button</p>
</div>

<div align="center">
	<img align="center" src="Images/Old/lukkelengine-03-12-3-changecolors.png" width=740 height=440>
<p align="center">Changed color of selected object</p>
</div>

<div align="center">
	<img align="center" src="Images/Old/lukkelengine-03-12-4.png" width=740 height=440>
<p align="center">View of the platform</p>
</div>

---

## February, 2023
### February 26

<div align="center">
	<img align="center" src="Images/Old/engine-26-2-2023-pic1.png" width=740 height=440>
</div>
<p align="center">Scene overview with a cube</p>

<br>
<div align="center">
	<img align="center" src="Images/Old/engine-26-2-2023-pic2.png" width=740 height=440>
<p align="center">The same cube but from a closer distance</p>
</div>

<br>

--- 
