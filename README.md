# Lightact-UE4
A set of functionalities enabling integration of Unreal Engine with Lightact media server.

## Installation
**Please note: this plugin has been tested with Unreal Engine 4.19. It does not work (yet) with 4.20.**

When you download the plugin files you need to Regenerate Visual Studio project files. You do that by right-clicking on the .uproject file. If you don't see this option, your UE project does not contain any code.
If this is the case you should add a dummy C++ class first to the project.

When you are done, double click on the .uproject file to open it in the UE Editor. Alternatively, open the project through Epic Launcher. You'll probably encounter warnings about missing plugin dll. You should rebuild the dlls.

When you are done and the UE Editor opens, make sure the Plugin is enabled.

## Usage
The plugin adds a number of new Blueprint nodes. They are all inside *Lightact* category. They have different purposes.

### Nodes for reading from shared memory
- **Shared memory JSON to string map**: this node reads from shared memory (Handle Name) of size (Handle Size). It expects the data to be in JSON format. The data is then converted to *Map of Strings to Strings*.

### Nodes for writing to shared memory
- **Open shared memory**: this node is used to create shared memory handle. It should be called once before all other nodes for writing to shared memory. In most cases you'll want to connect it to **Event BeginPlay** node.
- **Write to shared memory**: This node is used to write data to shared memory handle. It takes in a *Map of Strings to Strings* and converts it to JSON before writing it to shared memory.
- **Close shared memory**: this node is for closing shared memory handle. It should be called once before quitting the game. The *Handle Name* property should be the same as in the *Open shared memory* node. In most cases you'll want to connect it to **Event EndPlay** node.

## Additional help
Additional help is available on:
- [Lightact YouTube UE Playlist](https://www.youtube.com/playlist?list=PLcNPGta1d2XDcSsz8zcW0f2lPSawnW3mR)
- [User Guide](https://support.lightact-systems.com/unreal-engine-and-lightact-media-server-integration/)
