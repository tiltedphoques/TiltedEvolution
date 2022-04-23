# Tilted Online
![Build status](https://github.com/tiltedphoques/TiltedEvolution/workflows/CI/badge.svg?branch=master) [![Discord](https://img.shields.io/discord/247835175860305931.svg?label=&logo=discord&logoColor=ffffff&color=7389D8&labelColor=6A7EC2)](https://discord.gg/skyrimtogether)

<img src="https://avatars.githubusercontent.com/u/52131158?s=200&v=4" align="right"
     alt="Size Limit logo by Anton Lovchikov" width="110" height="100">

Tilted Online  is a framework created to enable multiplayer in Bethesda games, currently supporting **Skyrim Special Edition** and eventually **Fallout 4**.

## Getting started
To play Tilted Online, grab the latest released binaries from the [releases page](https://github.com/tiltedphoques/TiltedOnline/releases).

Check out the [build guide](https://wiki.tiltedphoques.com/tilted-online/coding/build-guide) for setup and development info on the project. When writing code, check the [code guidelines](https://wiki.tiltedphoques.com/tilted-online/coding/code-guidelines)
 and make sure to run clang-format!

## Contributing
Have some experience in C++, and want to help advance the project faster? Contribute!
- Check the issues, it's a good place to start when you don't know what to do.
- Fork the repository and create pull requests to this repository.
- Try to keep your code clean, following the [code guidelines](https://wiki.tiltedphoques.com/tilted-online/coding/code-guidelines).

## Main project source tree

* [**client/**](./Code/client): Sources for the SkyrimSE and FO4 clients.
* [**launcher/**](./Code/launcher): Game starter/updater.
* [**common/**](./Code/common): Common code shared between plugin and server.
* [**encoding/**](./Code/encoding): Net-message definitions.
* [**server/**](./Code/server): GameServer implementation.
* [**skyrim_ui/**](./Code/skyrim_ui): Source code for the ui, written in typescript. 
* [**tests/**](./Code/tests): Tests for the encoding and serialization code.
* [**tp_process/**](./Code/tp_process): Worker for CEF (Chromium Embedded Framework) overlay.

## License

Tilted Online is written by multiple independent parties. The copyright of a contribution is held by the contributing party.
