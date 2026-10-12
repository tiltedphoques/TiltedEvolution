# Agent Development Guide

**Skyrim Together** (codename TiltedEvolution) is a multiplayer mod for Skyrim Special Edition: co-op
questing, combat and exploration for 2-8 players.

- Stack: C++20 / MSVC, xmake 3.x (deps pinned in the root `xmake.lua`, `Libraries/Tilted*` are
  submodules). The UI is Angular (`Code/skyrim_ui`).
- Platforms: the client and launcher are Windows x64 only. The server also runs on Linux (amd64 and arm64),
  so keep server and shared code portable.
- Game versions: SE **1.7.104** is the main target (`kSupportedGameVersions` in `Code/client/main.cpp`).
  1.6.1170 is unofficially supported. Engine code and address IDs are effectively identical.
- Branches: open PRs against `dev`. `master` is the release branch.
- Logs: `build\windows\x64\<mode>\logs`.

## Architecture

**Client-authoritative, the server mostly relays.** Clients run the simulation. The server keeps a light world
model (players, parties, cells, which actors and objects exist, who owns them) and decides who receives what.
It never simulates Skyrim.

- The party leader effectively hosts the world. Quest progress, weather, time etc. come from the leader and
  are applied on the members' clients.
- Every synced actor or object has one owner client that runs the real AI and physics and streams its state.
  Other clients show a *remote* copy that replays it. Ownership moves when the owner leaves the cell or
  disconnects. Players are always owned by their own client.
- Clients are trusted. Validate on the server only where it matters (parties, ownership, admin commands).

**Engine integration (not an SKSE plugin).** `SkyrimTogether.exe` (`Code/immersive_launcher`) loads
`SkyrimSE.exe` into its own process, with the client (`Code/client`) linked in. `Code/client/main.cpp` loads
the Address Library, checks the game version, installs hooks, then loads SKSE if present so that other SKSE
plugins keep working.

- Resolve game functions and globals through Address Library IDs, never raw addresses.
- Hooks are MinHook detours added with `TP_HOOK` in a static `TiltedPhoques::Initializer` at the bottom of the
  engine file, calling the original through a saved pointer. Copy `Code/client/Games/Skyrim/Actor.cpp`.
- We don't use CommonLibSSE. Engine types are our own reversed headers in `Code/client/Games/Skyrim/**`
  (`Actor`, `TESObjectREFR`, `ExtraData`, `Havok`...), with layouts pinned by `static_assert(offsetof(...))`/
  `sizeof`. Add members and asserts as needed. A local CommonLibSSE checkout is fine as a reference; in its
  `Offsets*.h` the 1.6.x IDs are in the first `#ifdef SKYRIM_SUPPORT_AE` block.
- `GameFiles/Skyrim` holds what ships alongside the binaries: the ESPs, Papyrus scripts, meshes and behavior
  files.

**Data flow.**
1. Upstream: an engine hook or per-frame service poll notices a change. Hooks often run on engine worker
   threads, so queue the work onto the main thread through the runner service. A client service then sends a
   request through the transport service.
2. Server: a service handles the request with the sending player, updates its world model and relays a
   `Notify*` to everyone, players in range or the party, usually excluding the sender.
3. Downstream: a client service subscribed to the `Notify*` applies it to the game. While applying remote
   changes, guard against local hooks echoing them back upstream (existing services show how).

**ECS (`entt`).** Client and server each have a `World` (an `entt::registry` subclass) and an `entt::dispatcher`.

- **Services** (`Code/{client,server}/Services`): long-lived singletons in `World::ctx()` holding feature
  logic. They connect in their constructors (`aDispatcher.sink<T>().connect<&Svc::Fn>(this)`) and keep the
  connections in `entt::scoped_connection` members.
- **Components** (`Code/{client,server}/Components`): plain data. Data that belongs to an actor or object
  goes in a component on its entity. Use a service-side map keyed by form ID only if there's no other
  option. On the client, `FormIdComponent` links an entity to its form and `LocalComponent`/`RemoteComponent`
  mark the owner side.
- **Systems** (`Code/client/Systems`): near-stateless helpers over components (interpolation, animation...).
- **Events** (`Code/{client,server}/Events`): small structs passed through the dispatcher.

## Network protocol

- Transport is `Libraries/TiltedConnect` (Valve GameNetworkingSockets). All messages live in `Code/encoding`
  (`SkyrimEncoding`), shared by client and server. New messages follow the existing ones' naming, structure
  and registration.
- Reuse payload types from `Code/encoding/Structs`: `GameId` (`ModId` + `BaseId`, **never raw runtime form
  IDs**), `Vector3_NetQuantize`, `Rotator2_NetQuantize`.
- **No backward compatibility:** the client sends `BUILD_COMMIT` in `AuthenticationRequest` and the server
  rejects any mismatch (`kWrongVersion`), so reorder or change opcodes and fields freely.
- Watch bandwidth. Sends are reliable by default; send high-rate, loss-tolerant data (movement) unreliably.

## Building

From the repo root (VS 2022 developer shell or PowerShell with MSVC):

```sh
xmake f -m releasedbg -y                 # configure (debug | releasedbg | release); first run fetches deps
xmake -y                                 # build everything
xmake build SkyrimTogetherClient         # one target; also SkyrimImmersiveLauncher, SkyrimTogetherServer
xmake build TPTests && xmake run TPTests # main unit tests (Catch2)
xmake project -k vsxmake                 # regenerate the VS solution; rerun after adding/removing files
```

- The solution is `vsxmake2022/<repo dir>.sln`. Most contributors build and debug from it.
- `Build.bat` does the full dev build including the UI. UI only:
  `cd Code/skyrim_ui && pnpm install && pnpm deploy:production`. `xmake install -o distrib` gathers binaries.
- Main targets:
  - `SkyrimImmersiveLauncher` → `SkyrimTogether.exe`, with the `SkyrimTogetherClient` static lib linked in
  - `SkyrimTogetherServer` → `STServer.dll`, run by `SkyrimServerRunner` → `SkyrimTogetherServer.exe`
  - `ImmersiveElf` → `EarlyLoad.dll` (launcher early loader), `TPProcess` (UI overlay helper process)
  - Shared static libs: `SkyrimEncoding`, `CommonLib`, `BaseLib`
  - Tests: `TPTests`, plus `Console_Tests`, `ESLoader_Tests` and `Resources_Tests` for the component libs
    in `Code/components`

## Testing

- **Most gameplay features can only be verified by hand:** run a local server plus two or more game clients
  on one machine and exercise the feature from both sides (non-master builds write a log per instance).
  Say plainly when a change still needs an in-game test. Compiling is not proof that it works.
- Add unit tests only for mission-critical, Skyrim-independent code: protocol encoding, timing and
  interpolation math, pure data structures. Such code can be compiled into `TPTests`. Ask before writing 
  tests for gameplay code.

## Reverse engineering

- **IMPORTANT: if a task depends on engine behavior the source doesn't show (hook points, struct layouts,
  vtable slots, threading) and no IDA or Ghidra MCP tools are available, stop before implementing.**
  Say what needs verifying in the binary, warn that a source-only version may be wrong or suboptimal (guessed
  offsets, wrong hook point, missed side effects), and ask whether to go ahead.
- With an MCP, check the Skyrim SE 1.6.1170 database first. Use 1.7.104 only when something differs.
- Map Address Library IDs to IDB addresses and back with `python Tools/Scripts/addrlib.py <decimal id | IDB address>...`
  (1.6.1170 by default, `-v 1.7.104` for another version; usage at the top of the script).
- `Tools/Scripts` has other helpers for addresses, RTTI dumps and IDB matching.

## Conventions

- Match the surrounding style. Hungarian-style prefixes where the file uses them: `a` arguments, `c` const,
  `p` pointers, combined as needed (`acEvent`, `apActor`). No `r` prefix for references.
- Members use `m_`. Globals, including file-scope `static`s, use `g_`. Types and functions are `PascalCase`,
  and most functions are `noexcept`. Reversed engine types keep the game's names (`hCaster`, `slotType`).
- No exceptions and no throwing STL calls. No `auto` for primitive types.
- Use `TiltedPhoques` containers (`String`, `Vector`, `UniquePtr`...) where the surrounding code does. Log
  with `spdlog`.
- Gate debug-only UI and behavior with `IS_MASTER` / `DEBUG`. Mod compatibility shims live in
  `Code/client/ModCompat`.
- Commits: short imperative title describing the user-visible effect ("Fix X when Y", "Stop X doing Y"), and
  a short body explaining why.
