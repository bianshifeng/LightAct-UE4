# LightAct-UE4
A set of functionalities enabling integration of Unreal Engine with [LightAct media server](https://lightact-systems.com).

## Installation
**This plugin has been maintained and updated to work with Unreal Engine 4.19, 4.20, 4.21, 4.22, 4.23 and 4.24 together with Visual Studio 2017. Please note we can only guarantee compatibility with the latest UE version mentioned.**

### Step 1: Clone or Download the repository
Clone the repository in the Plugins folder of your project.
If you don't know what 'Clone' means, just download the ZIP archive and copy the folder to the *Plugins* folder in your UE project folder. For a more detailed help see [Installing UE plugins video tutorial](https://www.youtube.com/watch?v=85M2BB-Ct9g&list=PLcNPGta1d2XDcSsz8zcW0f2lPSawnW3mR&t=195s&index=2) or [Installing UE4 Plugins User Guide](https://lightact.io/docs/installing-ue4-plugins/).

### Step 2: Regenerate project files and build the plugin
When you download the plugin files you need to *Regenerate Visual Studio project files*. You do that by right-clicking on the .uproject file. If you don't see this option, your UE project does not contain any code.
If this is the case you should add a dummy C++ class first to the project.

When you are done, double click on the .uproject file to open it in the UE Editor. Alternatively, open the project through Epic Launcher. You'll probably encounter warnings about missing plugin dll. You should click *Yes* to rebuild the dlls.


### Step 3: Enabling the Plugin
When you are done and the UE Editor opens, make sure the Plugin is enabled. You might need to restart the UE Editor. If you are successful you'll see a new *LightAct* node category in your blueprints.

## Usage
The plugin adds a number of new Blueprint nodes. You'll find them all inside *LightAct* category. 

Below, we list some (but not all) of them.

### Nodes for reading from shared memory
#### Shared memory JSON to string map
![sharedMemoryJSONToStringMap.png](Resources/sharedMemoryJSONToStringMap.png)

This node reads from shared memory (**Handle Name**) of size (**Handle Size**). It expects the data to be in JSON format. The data is then converted to *Map of Strings to Strings*.

### Nodes for writing to shared memory
#### Open shared memory
![openSharedMemory.png](Resources/openSharedMemory.png)

This node is used to create shared memory handle. It should be called once before all other nodes for writing to shared memory. In most cases you'll want to connect it to **Event BeginPlay** node.
#### Write to shared memory
![writeToSharedMemory.png](Resources/writeToSharedMemory.png)

This node is used to write data to shared memory handle. It takes in a *Map of Strings to Strings* and converts it to JSON before writing it to shared memory.
#### Close shared memory
![closeSharedMemory.png](Resources/closeSharedMemory.png)

This node is for closing shared memory handle. It should be called once before quitting the game. The **Handle Name** property should be the same as in the **Open shared memory** node. In most cases you'll want to connect it to **Event EndPlay** node.

### Utility nodes
#### String to Vector
![stringToVector.png](Resources/stringToVector.png)

This node expects a string representing a 3-component vector (a standard *Vector* variable in UE) and outputs a *Vector* variable. It is useful if you want to transfer a *Vec3* variable from LightAct to a *vector* variable in UE.
#### String to Vector Array
![stringToVectorArray.png](Resources/stringToVectorArray.png)

This node expects a string representing an array of 3-component vectors (standard *Vector* variable in UE) and outputs an *Array of Vectors* variable. It is useful if you want to transfer an *Array of Vec3* variable from LightAct to *Array of Vectors* variable in UE. 

For example, [Find circles](https://lightact.io/docs/find-circles-and-contours/) node in LightAct outputs an *Array of Vec3* where x represents x-coordinate of circle's center, y represents y-coordinate of circle's center and z represents the radius of the circle.
#### String to Integer Array
![stringToIntArray.png](Resources/stringToIntArray.png)

This node expects a string representing an array of integers delimited with a Delimiter. It outputs an *Array of Integers*. It is useful, for example, if you want to transfer a *Art-Net* channels from LightAct to UE.
#### String to Float Array
![stringToFloatArray.png](Resources/stringToFloatArray.png)

This node expects a string representing an array of floats delimited with a Delimiter. It outputs an *Array of Floats*. It is useful, for example, if you want to transfer the results of *FFT analysis* (audio spectral analysis)  from LightAct to UE.
#### Extrude Contours
![extrudeContours.png](Resources/extrudeContours.png)

This node expects an *Array of Vectors* where each vector in the array represents _[x,y,i]_ of a contour, where _x_ is the x coordinate, _y_ is the y coordinate and _i_ is the index of a contour. Therefore the **Contours** *Array of Vectors* input can represent several contours. 

The node outputs an *Array of Vectors* representing **Vertices** of a mesh and an *Array of Integers* representing the **Triangles**. These can be connected into a **Create Mesh Section** node to create a mesh. Therefore, **Extrude Contours** node is usually used to 3D meshes in Unreal Engine based on the outputs of [Find contours](https://lightact.io/docs/find-circles-and-contours/) node in LightAct.

## Tips
In most cases you'll use just one *Handle Name* throughout your project as you can send many variables through that handle at the same time. In most cases, you'll connect a single **Open Shared Memory** in your main level blueprint (or elsewhere) and connect it to **BeginPlay** node and use one **Close Shared Memory** and connect it to **EndPlay** node.

## Troubleshooting
If you encounter any build errors, try opening the *.sln* file in Visual Studio and do a *Clean solution* and then *Build solution*. If these 2 actions succeeded you should be able to open the project.
If you have any questions, please ask them on [LightAct Answerhub](https://answerhub.LightAct.io/)

## Additional help
Additional help is available on:
- [LightAct Support](https://lightact.io/support) : Head to our Support page where you have a User Guide, a Light Academy video tutorials and other resources.   