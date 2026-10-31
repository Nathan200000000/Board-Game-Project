# C++ Board Game — Battle for Nexus

A turn-based strategy board game written in C++. Two factions — the **Alliance of Light** and the **Coalition of Shadows** — battle across a 10×10 grid on the planet Nexus, gathering resources, avoiding hazards, trading at markets and hunting for hidden cheat tiles. First to **100 resources** wins.

**Play it in your browser:** the game is compiled to WebAssembly and deployed on Vercel (see the repo's website link).

## Features

- Player vs Player or Player vs AI
- 10×10 board with Resource, Safe Zone, Hazard, Market and Cheat tiles
- Eight movement directions, 1–6 steps per turn
- Market trading (5 resources = 1 soldier)
- Cheat codes: `RESOURCES`, `SOLDIERS`, `TELEPORT`, `REVEAL`, `SABOTAGE`
- Save / load game (`savegame.txt`)

## Build and run natively

```bash
g++ -std=c++17 -Wall -o game main.cpp Game.cpp Player.cpp
./game
```

## Project structure

| File | Purpose |
| --- | --- |
| `main.cpp` | Game setup (mode, teams) and win check |
| `Game.h / Game.cpp` | Board, turns, tile effects, market, cheats, save/load |
| `Player.h / Player.cpp` | Player state, movement, AI moves |
| `Tile.h` | Tile types |
| `web/` | Browser version (WebAssembly build + terminal page) |

## Browser version

`web/build.sh` compiles the unchanged C++ sources to `web/public/game.wasm` with the [WASI SDK](https://github.com/WebAssembly/wasi-sdk). The page (`web/public/index.html`) shows the game in an [xterm.js](https://xtermjs.org/) terminal, and `web/public/worker.js` runs it in a Web Worker with a small WASI runtime so `std::cin` can wait for your input. Saved games are stored in the browser's localStorage.

To rebuild after changing the C++ code:

```bash
WASI_SDK=/path/to/wasi-sdk web/build.sh
```

`vercel.json` serves `web/public` as a static site and sets the cross-origin isolation headers the worker needs.
