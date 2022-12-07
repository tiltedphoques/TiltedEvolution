# Plugin spec

> ⚠️ Make sure to compile your shared libraries with the same compiler as the server.

## C++ API Boundary Design

Export a PluginDescriptor struct called `TT_PLUGIN` that has the following fields:

- `magic`: The unique identifier of the plugin symbol. This should be set to `PLGN`.
- `structSize`: The size of the struct. This should be set to `sizeof(PluginDescriptor)`. This is defined for future api change compatibility.
- `pluginVersion`: The version of the plugin. This is a user defined value.
- `pluginName`: The name of the plugin.
- `authorName`: The name of the author.
- `flags`: The flags of the plugin. This is a bitfield of the following flags:
  - `kNone`: No flags.
  - `kInternal`: Reserved for internal "tilted" use.
  - `kHotReload`: The plugin supports reloading at runtime.
- `pCreatePlugin`: A function pointer to a function that creates the plugin instance. This is used for managing the lifetime of the plugin instance on your own heap.
- `pDestroyPlugin`: A function pointer to a function that destroys the plugin instance. This is used for managing the lifetime of the plugin instance on your own heap.

To instantiate the plugin, define a CreatePlugin function that returns a PluginInterface struct. This struct has the following functions:

- `~PluginInterface001` (destructor): This is called when the plugin is unloaded. This is used for cleaning up any resources that the plugin has allocated.
- `GetVersion`: This is called to resolve the version of the PluginInterface. This should return `001` or greater.
- `Initialize`: This is called when the plugin is loaded. This is used for initializing the plugin.
- `Shutdown`: This is called when the plugin is unloaded. This is used for cleaning up any resources that the plugin has allocated.
- `OnTick`: This is called every tick. This is used for updating the plugin.
- `OnMessage`: This is called when a message is received. This is used for handling messages.
