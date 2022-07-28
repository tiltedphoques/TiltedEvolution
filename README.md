# Tilted Online
![Build status](https://github.com/tiltedphoques/TiltedEvolution/workflows/Build%20windows/badge.svg?branch=master) [![Discord](https://img.shields.io/discord/247835175860305931.svg?label=&logo=discord&logoColor=ffffff&color=7389D8&labelColor=6A7EC2)](https://discord.gg/skyrimtogether)

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
