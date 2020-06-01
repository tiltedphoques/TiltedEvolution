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

Use `.bat` on Windows and `.sh` on Linux. On Windows, VS2019 is preferred.

**Important:** You will need to do this step every time after you have merged from the main repository to avoid conflicts.

### Compiling the project

In the project, you can switch configurations of `Win32`/`x64` and `Skyrim`/`Fallout4`. In the end, all project should be compiled successfully.

## Linking the files to the game

### Configuration file

First it is required to copy over the ``tp_loader.conf`` file located in ``Mod\Build`` to the relevant bin directory (``Mod\Build\bin\<x32|x64>``). For Fallout, you will need to edit the following parameters to the following values:

```
GameId64=377160
GamePath64=Full/Path/To/Fallout4.exe
DllRelease=Full/Path/To/FalloutTogether.dll
DllDebug=Full/Path/To/FalloutTogether_d.dll
```

For Skyrim and Skyrim SE, you will only need to add the full path in front of the `Skyrim(SE).exe` and `.dll`.

### (Optional:) Shortcuts to binaries

For convenience, create a shortcut link of the `Loader.exe` and the `GameServer.exe`, directly onto your task bar.

### Setting up CEF

We currently use CEF for the main UI. To get CEF running, you will need to copy over the CEF files located in ``Mod\Libraries\TiltedUI\ThirdParty\CEF\bin\<Win32|Win64>\Release`` to the root directory of the compiled binaries: ``Mod\Build\bin\<x32|x64>``.

## Verifying

If everything has worked as planned, a Tilted Reverse Console will pop up and the Imgui UI will be displayed on top of the game. After launching the game's Server.exe, you should be able to connect in-game by hitting the F7 key.

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
