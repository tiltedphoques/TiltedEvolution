# Tilted Online Nemesis

The first couple of commits on the fork are to retain the work of Edho08 and Spvvd and give appropriate credit to them for their initial efforts to support animation mods with Skyrim Together Reborn.
Subsequent commits are the efforts of rfortier (Ujave on Discord) and MostExcellent / MagicWizardMan.

Sometimes animations still don't sync, restarting game+server or rerunning Nemesis and syncing the output with your friends can fix it in most cases.

Works only with Skyrim Version 1.6.640.0 and you can only join servers running this build of STR. It might work with the latest Skyrim and the correct address library, but it has barely been tested.
And the STR team has not released 1.6 yet.

This is a continuation of Edho08's original work, later updated by Spvvd and Ragley. This version by RFortier and MostExcellent addeds these goals:
* Pure feature branch to make it easily rebaseable. 
* Minimal intrusion in the base code.
* Feature is conditionally compiled. Enabled by a single commit in this history which will show the one line to change to turn it off.
* Tries to remove as much of the complexity for modders as possible. To mod a behavior you don't need to know any your new hash of your modded behavior, the game calculates them.
* It's to your advantage to know the _original game behavior hash_ you are moddifying, though; if you do, you only need to list the behavior variables your mod needs, 
the rest that the game devs selected will be picked up automatically for you. This helps give your mod STR version-independence, and more importantly means combining
your mod with others will work.

# TODO
* Currently, BehaviorVar.cpp works, but it's blobby; when I get some time I'll simplify it.
* Build up the fomod for a proper mod installer again, and document the simplified behaviorvar support.



# Tilted Online
![Build status](https://github.com/tiltedphoques/TiltedEvolution/workflows/Build%20windows/badge.svg?branch=master) [![Build linux](https://github.com/tiltedphoques/TiltedEvolution/actions/workflows/linux.yml/badge.svg)](https://github.com/tiltedphoques/TiltedEvolution/actions/workflows/linux.yml)  [![Discord](https://img.shields.io/discord/247835175860305931.svg?label=&logo=discord&logoColor=ffffff&color=7389D8&labelColor=6A7EC2)](https://discord.gg/skyrimtogether)

<img src="https://avatars.githubusercontent.com/u/52131158?s=200&v=4" align="right"
     alt="Size Limit logo by Anton Lovchikov" width="110" height="100">

Tilted Online is a framework created to enable multiplayer in Bethesda games, currently supporting **Skyrim Special Edition** and eventually **Fallout 4**.

## Getting started
To play Tilted Online, go to the [nexus page](https://www.nexusmods.com/skyrimspecialedition/mods/69993).

For general information, go to the [Tilted Online Wiki](https://wiki.tiltedphoques.com/tilted-online/).

Check out the [build guide](https://wiki.tiltedphoques.com/tilted-online/technical-documentation/build-guide) for setup and development info on the project. When writing code, check the CODE_GUIDELINES.md and make sure to run clang-format!

## Reporting bugs
If you would like to report a bug please report them in the "Issues" tab on this page. Detailed and reproducible bug reports are of great importance for the development of the project.

## Contributing
Have some experience in C++, and want to help advance the project faster? Contribute!
- Check the issues, it's a good place to start when you don't know what to do.
- Fork the repository and create pull requests to this repository.
- Create pull requests to the "dev" branch, not to master or prerel.
- Try to keep your code clean, following the code guidelines.

## Main project source tree

* [**client/**](./Code/client): Sources for the SkyrimSE and FO4 clients.
* [**immersive_launcher/**](./Code/immersive_launcher): Game starter/updater.
* [**common/**](./Code/common): Common code shared between plugin and server.
* [**encoding/**](./Code/encoding): Net-message definitions.
* [**server/**](./Code/server): GameServer implementation.
* [**skyrim_ui/**](./Code/skyrim_ui): Source code for the ui, written in typescript. 
* [**tests/**](./Code/tests): Tests for the encoding and serialization code.
* [**tp_process/**](./Code/tp_process): Worker for CEF (Chromium Embedded Framework) overlay.

## License
[![GNU GPLv3 Image](https://www.gnu.org/graphics/gplv3-127x51.png)](http://www.gnu.org/licenses/gpl-3.0.en.html)

Tilted Online is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
