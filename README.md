# Skyrim Together
#### codename "TiltedEvolution"
[![GitHub release](https://img.shields.io/github/v/release/tiltedphoques/TiltedEvolution?color=23314E)](https://github.com/tiltedphoques/TiltedEvolution/releases)
[![Nexus Mods](https://img.shields.io/badge/Nexus%20Mods-download-FB923C?logo=nexusmods&logoColor=white)](https://www.nexusmods.com/skyrimspecialedition/mods/69993)
[![Language](https://img.shields.io/github/languages/top/tiltedphoques/TiltedEvolution?logo=cplusplus&color=F34B7D&logoColor=white)](https://github.com/tiltedphoques/TiltedEvolution)
[![Build status](https://github.com/tiltedphoques/TiltedEvolution/actions/workflows/windows.yml/badge.svg)](https://github.com/tiltedphoques/TiltedEvolution/actions/workflows/windows.yml)
[![Build linux](https://github.com/tiltedphoques/TiltedEvolution/actions/workflows/linux.yml/badge.svg)](https://github.com/tiltedphoques/TiltedEvolution/actions/workflows/linux.yml)
[![Discord](https://img.shields.io/discord/247835175860305931.svg?label=&logo=discord&logoColor=ffffff&color=7389D8&labelColor=6A7EC2)](https://discord.gg/skyrimtogether)

<img src="https://avatars.githubusercontent.com/u/52131158?s=200&v=4" align="right"
     alt="Size Limit logo by Anton Lovchikov" width="110" height="100">

TiltedEvolution is a framework created to enable multiplayer in Bethesda games, currently supporting **Skyrim Special Edition**.

## Getting started
To play Skyrim Together, go to the [nexus page](https://www.nexusmods.com/skyrimspecialedition/mods/69993).

For general information, go to the [TiltedEvolution Wiki](https://wiki.tiltedphoques.com/tilted-online/).

Check out the [build guide](https://wiki.tiltedphoques.com/tilted-online/technical-documentation/build-guide) for setup and development info on the project. When writing code, check the CODE_GUIDELINES.md and make sure to run clang-format!

## Fallout Together development branch

`dev-ft` contains the Fallout 4 port and periodically merges `dev`. Configure with
`xmake f --game=fallout4 -m release -y`, then run `xmake -y` to build the Windows
client, launcher and server.

### F4SE

Fallout Together loads an installed [F4SE](https://f4se.silverlock.org/) core before
the game's entry point, using the same launcher integration as Skyrim's SKSE.
For the supported Fallout 4 runtime **1.11.240**, install F4SE **0.7.9** (or a
compatible newer build for that runtime): put `f4se_1_11_240.dll` beside
`Fallout4.exe` and install the archive's `Data` files, including its scripts.
Install compatible native plugins in `Data/F4SE/Plugins`, following each mod's
instructions. Launch **FalloutTogether.exe** and select `Fallout4.exe`; the
separate `f4se_loader.exe` is not needed. Without F4SE installed, FT continues
without the extender. The client log reports whether its startup hooks loaded.

The FT server's `[ModPolicy]` setting `bAllowF4SE` controls admission of F4SE
clients and defaults to `true`. The legacy `bAllowSKSE` setting must also be
`true`; existing servers that disabled it continue rejecting extender clients.
Rejection messages identify F4SE by name.

F4SE plugin loading does not synchronize arbitrary plugin state or restore FT's
disabled custom Papyrus synchronization. Individual plugins and actual game
startup still require an in-game compatibility check.

On Windows, `xmake build ScriptExtenderTests && xmake run ScriptExtenderTests`
runs isolated DLL fixtures against the real launcher path setup and extender
loader. These checks cover version/runtime validation, plugin dependency lookup
and one-time startup without a game installation; they do not run F4SE itself.

## Reporting bugs
If you would like to report a bug please report them in the "Issues" tab on this page. Detailed and reproducible bug reports are of great importance for the development of the project.

## Contributing
Have some experience in C++, and want to help advance the project faster? Contribute!
- Check the issues, it's a good place to start when you don't know what to do.
- Fork the repository and create pull requests to this repository.
- Create pull requests to the "dev" branch, not to master.
- Try to keep your code clean, following the code guidelines.

## Main project source tree

* [**client/**](./Code/client): Skyrim SE client: engine hooks, reverse-engineered game types and the sync logic.
* [**immersive_launcher/**](./Code/immersive_launcher): `SkyrimTogether.exe`, which starts the game with the client loaded.
* [**immersive_elf/**](./Code/immersive_elf): `EarlyLoad.dll` helper DLL for the launcher. Detects if Mod Organizer 2 was injected first.
* [**tp_process/**](./Code/tp_process): Worker for the CEF (Chromium Embedded Framework) overlay.
* [**skyrim_ui/**](./Code/skyrim_ui): In-game UI, written in Angular/TypeScript.
* [**server/**](./Code/server): Game server (`STServer.dll`).
* [**server_runner/**](./Code/server_runner): Dedicated server executable that hosts the server DLL.
* [**encoding/**](./Code/encoding): Network message and struct definitions shared by the client and server.
* [**common/**](./Code/common), [**base/**](./Code/base): Shared utilities.
* [**components/**](./Code/components): Shared modules (console/settings, ESP loader, resources, ImGui).
* [**tests/**](./Code/tests): Unit tests.
* [**GameFiles/**](./GameFiles): Plugins, scripts and assets shipped with the mod.
* [**Libraries/**](./Libraries): Networking, hooking, reverse-engineering and UI libraries (git submodules).
* [**Tools/**](./Tools): Reverse-engineering and address scripts.

## License
[![GNU GPLv3 Image](https://www.gnu.org/graphics/gplv3-127x51.png)](http://www.gnu.org/licenses/gpl-3.0.en.html)

TiltedEvolution is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
