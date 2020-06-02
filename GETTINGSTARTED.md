# Getting started

## Cloning the repository

Preferred way is cloning using SSH, so that you don't need to re-authenticate all the time.

When asked to initialize Git LFS, say that you want it initialized.

### CLI

Run the following command: ``git clone --recursive https://github.com/tiltedphoques/TiltedOnline.git``

## Setting up the environment

### Generating the project files

Before we generate the project files, synchronize and initialize the submodules that are required by the project. This is required so that the submodules are used in the generation process. In a CLI, run the following commands:

```
git submodule sync --recursive
git submodule update --init --recursive
```

After updating the submodules, you're required to generate the project files using the ``MakeVS201?Projects.*`` file.

Use `.bat` on Windows and `.sh` on Linux. On Windows, VS2019 is preferred. Future versions of the project could cause issues with older VS versions, since it could rely on functions not available in those.

**Important:** You will need to do this step every time after you have merged from the main repository to avoid conflicts.

### Compiling the project

In the project, you can switch the configurations between `Skyrim`/`Fallout4`. In the end, all projects should be compiled successfully (maybe).

## Linking the files to the game

### Configuration file

First it is required to copy over the ``tp_loader.conf`` file located in ``Mod\Build`` to the relevant bin directory (``Mod\Build\bin\x64``). For Fallout, you will need to edit the following parameters to the following values:

```
GameId64=377160
GamePath64=Full/Path/To/Fallout4.exe
DllRelease=Full/Path/To/FalloutTogether.dll
DllDebug=Full/Path/To/FalloutTogether_d.dll
```

For Skyrim and Skyrim SE, you will only need to add the full path in front of the `SkyrimSE.exe` and `.dll`. 
Adding the fullpath to the .dll is only neccessary if you moved it in your Skyrim/Fallout4 installation folder.

### (Optional:) Shortcuts to binaries

For convenience, create a shortcut link of the `Loader.exe` and the `GameServer.exe`, directly onto your task bar.

### Setting up CEF

We currently use CEF for the main UI. To get CEF running, you will need to copy over the CEF files located in ``Mod\Libraries\TiltedUI\ThirdParty\CEF\bin\Win64\Release`` to the root directory of the compiled binaries: ``Mod\Build\bin\x64``.

## Verifying

If everything has worked as planned, a Tilted Reverse Console will pop up and the Imgui UI will be displayed on top of the game. After launching the game's Server.exe, you should be able to connect in-game by using the UI in the top left corner. Pressing RCTRL thrice should show your mouse, so youre able to use the UI.

### (Optional: ) Server parameters

Following are the arguments to start a server, they are optional.

Eg. SkyrimTogetherServer.exe --name "SkyrimTogether Server" --port "10578" --premium

```
  -p, --port N     port to run on (default: 10578)
      --premium    Use the premium tick rates (60 ticks instead of 20 ticks)
  -h, --help       Display the help message
  -n, --name arg   Name to advertise to the public server list
  -v, --verbose    Log verbose
  -t, --token arg  The token required to connect to the server, acts as a password
```

## Debugging

In Visual Studio, go to ``Debug -> Attach to process`` and select the game(s) ("TESV.exe", "SkyrimSE.exe" or "Fallout4.exe") and click on ``Attach``.

If you explicitly require a debugger, you can add the line ``Debug::WaitForDebugger();`` in the ``TiltedOnlineApp::TiltedOnlineApp()`` ctor. This will block the game from running on startup until a debugger has been attached.

## Working in this repository

From now on, whenever you want to make a change in the repository, you will first need to branch off the master branch. First check if the branch is fully up to date:

```
git checkout master
git fetch
git pull
```

Then create a new branch. Please use the ``feature-`` prefix so that it is clear that it is a temporary, in-progress development branch.

### CLI

Run the following command to create a new branch: ``git checkout -b feature-somenamehere``.

### GitHub for Desktop

In the application, go to ``Branch -> New branch...``. Give this an appropriate name (don't forget the prefix) and ensure that the branch is based on the ``master`` branch.

## Understanding the code

**Client** is the core mod it is broken down in Games/ Services/ and Systems/.
- Games/ contains all code that is skyrim and fallout 4 related, it mostly contains class structures and hooks to different parts of the engine
- Services/ contains the different services that handle the actual sync, display, gameplay
- Systems/ contains specific tasks like interpolation, consuming animations

**Server** is the ... server !
It doesn't contain much it's a translation layer more than anything at the moment.

When getting started, looking at the **TestService** in the client is a good start, it pretty much shows how to get a service to listen to update events, and to spawn a copy of yourself.

Once you get the hang of it **ScriptService** is interesting as it shows how to iterate over the world entities, retrieve the components and manipulate them correctly.
