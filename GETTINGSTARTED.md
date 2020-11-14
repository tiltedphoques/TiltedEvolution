# Getting started

## Cloning the repository

Choose or create a directory for the repository to get cloned into, such as your Desktop. This is the directory we'll be working out of, so pick somewhere convenient.

Using a CLI, run the following command in the directory from the previous step: `git clone --recursive https://github.com/tiltedphoques/TiltedOnline.git`

Once the command has finished executing, you should have a copy of this repository named `TiltedOnline` inside your project directory.

## Setting up the environment

### Requirements

As mentioned in the [Readme](https://github.com/tiltedphoques/TiltedOnline#windows), you will need [Visual Studio 2019](https://www.visualstudio.com/downloads/) (the community edition is freely available for download) and [Directx SDK](https://www.microsoft.com/en-us/download/details.aspx?id=6812) to build the project, aswell as [Node.js](https://nodejs.org/en/) for the scripts to run properly.

### Generating the project files

Before you generate the project files, you'll need to synchronize and initialize the submodules that are required by the project so that the submodules are used in the generation process. In a CLI, navigate to the `TiltedOnline` folder and run the following commands:

```
git submodule sync --recursive
git submodule update --init --recursive
```

After that's finished, generate the project files using one of the `MakeVS201?Projects.*` files within `TiltedOnline\Build` - use `.bat` on Windows and `.sh` on Linux.

VS2019 is preferred for Windows as future versions of this project could cause issues with older versions of VS in the case that the project makes use of VS2019 functions that weren't previously available.

**Important:** You will need to do this step every time you merge changes from the main repository to avoid conflicts.

### Compiling the project

Open the solution `Tilted Online Framework.sln` found in (`TiltedOnline\Build\projects`).
Once it has opened, make sure the solution configuration is set to `Release` instead of `Debug`.

Next, compile the project via the Menu Option `Build` -> `Build Solution`. 

If all goes well, everything should now be compiled. Should you encounter any errors, feel free to ask for help in the SkyrimTogether Discord #coding channel.

## Linking the files to the game

### Configuration file

Now that you've compiled the project, copy the `tp_loader.conf` file in `TiltedOnline\Build` to `TiltedOnline\Build\bin\x64`.

Open `tp_loader.conf` with your preferred text editor. For Skyrim SE, you will only need to edit the value of `GamePath64` to reflect the full path to your `SkyrimSE.exe` game executable.

However, for Fallout, you will need to edit the provided parameters to reflect the following values instead:

```
GameId64=377160
GamePath64=Full/Path/To/Fallout4.exe
DllRelease=FalloutTogether.dll
DllDebug=FalloutTogether_d.dll
```

### Setting up CEF

We currently use CEF for the main UI. To get CEF running, you will need to copy the CEF files in `TiltedOnline\Libraries\TiltedUI\ThirdParty\CEF\bin\Win64\Release` to `TiltedOnline\Build\bin\x64`, the directory of the compiled binaries.

### (Optional:) Shortcuts to binaries

For your convenience, it's recommended to create a shortcut to `TiltedOnline\Build\bin\x64\Loader.exe` and `TiltedOnline\Build\bin\x64\SkyrimTogetherServer.exe` (or `TiltedOnline\Build\bin\x64\FalloutTogetherServer.exe` for Fallout) either on your task bar or in the root folder containing `TiltedOnline`.

### (Optional: ) Server parameters

Instead of running the server executables directly, you can instead create a .bat or .sh file to run the executable with any of the following (optional) parameters:

```
  -p, --port N     port to run on (default: 10578)
      --premium    Use the premium tick rates (60 ticks instead of 20 ticks)
  -h, --help       Display the help message
  -n, --name arg   Name to advertise to the public server list
  -v, --verbose    Log verbose
  -t, --token arg  The token required to connect to the server, acts as a password
```

Eg. `SkyrimTogetherServer.exe --name "SkyrimTogether Server" --port "10578" --premium`

## Verifying

Launch `Loader.exe` to start up the mod, either directly or via a shortcut. If everything worked as intended, a Tilted Reverse Console will pop up and the Imgui UI will be displayed on top of your game. Once loaded into a save, run the corresponding server executable (or script, if you created one). You should now be able to connect in-game by using the UI in the top left corner; pressing RCTRL thrice should free your mouse so that you're able to interact with the UI. The UI is layered on top of one another by default, so you'll need to move the UI windows first to be able to connect.

## Debugging

In Visual Studio, go to `Debug` -> `Attach to process` and select the game(s) (`SkyrimSE.exe` or `Fallout4.exe`) then click `Attach`.

If you explicitly require a debugger, you can add the line `Debug::WaitForDebugger();` in the `TiltedOnlineApp::TiltedOnlineApp()` constructor. This will block the game from running on startup until a debugger has been attached.

## Working in this repository

From now on, whenever you want to make a change in the repository, you will first need to branch off the master branch. Using a CLI in the `TiltedOnline` directoy, first check if the master branch is fully up to date:

```
git checkout master
git fetch
git pull
```

Now you can create a new branch. Please use the `feature-` prefix so that it's clear that your branch is a temporary, in-progress development branch. Creating your branch can be done using one of two methods:

### CLI

Simply enter `git checkout -b feature-somenamehere` to have a branch created for you.

### GitHub for Desktop

In the application, go to ``Branch -> New branch...``. Give this an appropriate name (don't forget the prefix) and ensure that the branch is based on the `master` branch.

## Understanding the code

### Project structure

When it comes to the actual code (located in `TiltedOnline\Code`), the two primary folders of interest are `client` and `server`.

**Client** is the core of the mod and can be broken down into `Games`, `Services`, and `Systems`.
- `Games` contains all of the code that is Skyrim and Fallout 4 related, it mostly contains class structures and hooks to different parts of the engine
- `Services` contains the different services that handle the actual sync, display, and gameplay
- `Systems` contains specific tasks like interpolation and consuming animations

**Server** is the ... server !
It doesn't really contain much at the moment, it's a translation layer more than anything for the time being.

### Starting points

When getting started, a good place to begin is looking at the **TestService** in `client` as it demonstrates how to get a service to listen to update events and how to spawn a copy of yourself.

Once you wrap your head around that you can have a look at **ScriptService**, which shows how to iterate over the world entities, retrieve the components and manipulate them correctly.

------

And that's all! With that, you're on your way to making great contributions to the repository. As mentioned previously, if you have any questions or encounter any hurdles, feel free to use the #coding channel in the SkyrimTogether Discord!
