# LkEngine

1. [Structure](#Engine-Structure)
    * [Core](#Engine-Core)
        * [LObject](#Engine-Core-LObject)
    * [LkEditor (Engine Editor)](#Engine-Editor)
    * [LTesto (Test Framework)](#Engine-TestFramework)

---

# Structure <a id="Engine-Structure"></a>

The engine is split up into several modules: **Core**, **LkEditor**, **LTesto.**

## Core <a id="Engine-Core"></a>

### Macros
> Controversial topic, I know :100:

The engine utilize macros for different things, such as:
 * Static class registration
 * Dynamic object registration

The macros can be divided in to different types for their different uses.

| Macro Type | Usage |
| :-------- | :-------: |
| Header | Placed in class/struct definitions |
| Constructor | Placed in constructors | 

#### Header 
A **header** macro is used in a static context and is most often used to implement static and/or abstract functions.

#### Constructor
A **constructor** macro is used in dynamic contexts.  
An example is registering a created object instance to the garbage collector.

<br>

The engine macros can be viewed in the table below.

| Macro |  Type | Details |
| :-------- | :---: | :-------: |
| LCLASS | Header | Implements abstract LObject functions and static getters for the class type. |
| LSTRUCT | Header | :warning: Not done :warning: |
| LOBJECT_REGISTER | Constructor | :x: |


### LObject <a id="Engine-Core-LObject"></a>
> [LObject/Object.h](../LkEngine/Source/LkEngine/Core/LObject/Object.h)<br>
> [LObject/Object.cpp](../LkEngine/Source/LkEngine/Core/LObject/Object.cpp)

The base object class for the engine.  
All derivations of LObject must declare the **LCLASS** macro somewhere in the class declaration.

<!--
[TObjectPtr](../LkEngine/Source/LkEngine/Core/LObject/ObjectPtr.h) 
-->

#### Smart Pointer Implementation
The [LObject](../LkEngine/Source/LkEngine/Core/LObject/Object.h) class uses [TObjectPtr](../LkEngine/Source/LkEngine/Core/LObject/ObjectPtr.h) for reference counting.
<br>


### Events <a id="Engine-Events"></a>
Input events are reported from the active [GLFW](https://www.glfw.org/) context.

All events derive from the base event class [LEvent](../LkEngine/Source/LkEngine/Core/Event/Event.h).  

| Event | Category | Description |
| :---- | :----: | :---- |
| [LKeyPressedEvent](../LkEngine/Source/LkEngine/Core/Event/KeyEvent.h) |  Input | Contains info about a single key press |
| [LMouseButtonPressedEvent](../LkEngine/Source/LkEngine/Core/Event/MouseEvent.h) |  Input | Contains info about a pressed mouse button |


#### Event Queue
Events are placed in a [LEventQueue](../LkEngine/Source/LkEngine/Core/Event/EventQueue.h).

```cpp
LEventQueue EventQueue;
...

/* Execute all queued events, no matter the type. */
EventQueue.Process();

/* Only execute queued LMousePressedEvent events, skip the others. */
EventQueue.ProcessFiltered<LMousePressedEvent>();
```

### Delegates <a id="Engine-Delegates"></a>

The delegate implementation is found in [Delegate.h](../LkEngine/Source/LkEngine/Core/Delegate/Delegate.h).

| Macro Declaration | Type | Returns |
| :-------- | :------- | :---: |
| **LK_DECLARE_DELEGATE**(*Args...*) | Single | Nothing |
| **LK_DECLARE_DELEGATE_RET**(Name, ReturnType, *Args...*) | Single | ReturnType |
| **LK_DECLARE_MULTICAST_DELEGATE**(*Args...*) | Multicast | Nothing |

<br>

## LkEditor <a id="Engine-Editor"></a>
> The engine editor.


<br>

## LTesto<a id="Engine-TestFramework"></a>
> Test Framework for LkEngine


