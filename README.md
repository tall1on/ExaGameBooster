<p align="center">
  <a href="#">
    <img alt="Exatek" src="https://cdn.exatek.de/exatek/exa-partner.png" width="300" />
  </a>
</p>

# ExaGameBooster™

ExaGameBooster™ is a open-source process optimization service for games. This program optimizes core allocations
for games to improve performance and reduce lags. We support modern CPU architectures such as AMD 3D V-Cache™ and Intel
P+E cores.

This software is provided "as is" without warranty of any kind, either express or implied. Use at your own risk.

Current Version:
v2.1.0 ([download](https://github.com/Exatek-Germany/ExaGameBooster/releases/download/v2.1.0/ExaGameBooster_x64.zip)
via GitHub)

### Roadmap (as of 2023-02-26)

- v2.2.0: Launch Improvements for R9 7900X3D/7950X3D + Support for more Games
- v3.0.0: Improved Installer, New Uninstaller and Updater + Support for more Games

### Supported CPU's

> **_NOTE:_**  If your CPU is not in the list, you probably do not need this program. This program solves scheduler
> problems caused by multi-die CPUs.

#### AMD Ryzen Raphael (7000 Series)

| Version | Ryzen 9 7950X3D | Ryzen 9 7900X3D | Ryzen 9 7950X | Ryzen 9 7900X | Ryzen 9 7900 |
|---------|:---------------:|:---------------:|:-------------:|:-------------:|:------------:|
| 2.0.0   |        ✅        |        ✅        |       ✅       |       ✅       |      ✅       |
| 1.1.0   |        ✅        |        ✅        |       ✅       |       ✅       |      ❌       |
| 1.0.0   |        ✅        |        ❌        |       ✅       |       ❌       |      ❌       |

#### AMD Ryzen Vermeer (5000 Series)

| Version | Ryzen 9 5950X | Ryzen 9 5900X | Ryzen 9 5900 |
|---------|:-------------:|:-------------:|:------------:|
| 2.0.0   |       ✅       |       ✅       |      ✅       |
| 1.1.0   |       ✅       |       ✅       |      ❌       |
| 1.0.0   |       ✅       |       ❌       |      ❌       |

#### AMD Ryzen Matisse (3000 Series)

| Version | Ryzen 9 3950X | Ryzen 9 3900X | Ryzen 9 3900 |
|---------|:-------------:|:-------------:|:------------:|
| 2.0.0   |       ✅       |       ✅       |      ✅       |
| 1.1.0   |       ✅       |       ✅       |      ❌       |
| 1.0.0   |       ✅       |       ❌       |      ❌       |

#### Intel Raptor Lake (13th Gen)

| Version | i9 13900 | i7 13700 | i5 13600 | i5 13500 | i5 13400 | Mobile CPUs (4+ E-Cores) |
|---------|:--------:|:--------:|:--------:|:--------:|:--------:|:-----------:|
| 2.1.0   |    ✅     |    ✅     |    ✅     |    ✅     |    ✅     |      ✅      | 
| 1.0.0   |    ✅     |    ✅     |    ✅     |    ✅     |    ✅     |      ❌      | 

#### Intel Alder Lake (12th Gen)

| Version | i9 12900 | i7 12700 | i5 12600K | Mobile CPUs (4+ E-Cores) |
|---------|:--------:|:--------:|:---------:|:-----------:|
| 2.1.0   |    ✅     |    ✅     |     ✅     |      ✅      |
| 1.0.0   |    ✅     |    ✅     |     ✅     |      ❌      |

### Supported Operating Systems

ExaGameBooster™ is supported by Windows 10 and 11. We currently do not plan to extend the supported operating
systems.

### Supported Games

> **_NOTE:_**  We are constantly working to test and add new games. If you would like to see support for specific games,
> please create an issue on GitHub or email us at gamebooster@exatek.de.

| Game                                                     |         2.1.0         | 2.0.0 | 1.1.0 | 1.0.0 | Comments                                                                                                                                                                 |
|----------------------------------------------------------|:---------------------:|:-----:|:-----:|:-----:|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Assetto Corsa                                            |           ✅           |   ❌   |   ❌   |   ❌   |                                                                                                                                                                          |
| Call of Duty                                             |           ✅           |   ✅   |   ✅   |   ❌   |                                                                                                                                                                          |
| Counter Strike: Global Offensive                         |    ⏳ (in testing)     |   ❌   |   ❌   |   ❌   | Valve is positive about the program, but cannot assure us of VAC compatibility. We are currently testing VAC compatibility and will add CS:GO support in a later update. |
| Cyberpunk 2077                                           |           ✅           |   ✅   |   ✅   |   ✅   |                                                                                                                                                                          |
| DCS World                                                |           ✅           |   ❌   |   ❌   |   ❌   |                                                                                                                                                                          |
| Escape from Tarkov                                       | ❌ (awaiting approval) |   ❌   |   ❌   |   ❌   | As of 2023-02-26 we are still waiting for a response from Battleye. We will add support for Battleye games in a later update.                                            |
| F1 Manager 2022                                          |           ✅           |   ✅   |   ✅   |   ✅   |                                                                                                                                                                          |
| Farming Simulator 2022                                   |           ✅           |   ❌   |   ❌   |   ❌   |                                                                                                                                                                          |
| FIFA 23                                                  |           ✅           |   ❌   |   ❌   |   ❌   |                                                                                                                                                                          |
| GTA V                                                    |           ✅           |   ✅   |   ✅   |   ✅   |                                                                                                                                                                          |
| Genshin Impact                                           |           ✅           |   ❌   |   ❌   |   ❌   |                                                                                                                                                                          |
| God of War                                               |           ✅           |   ❌   |   ❌   |   ❌   |                                                                                                                                                                          |
| Half-Life: Alyx                                          |           ✅           |   ❌   |   ❌   |   ❌   |                                                                                                                                                                          |
| Hearthstone                                              |           ✅           |   ❌   |   ❌   |   ❌   |                                                                                                                                                                          |
| Hitman 3                                                 |           ✅           |   ✅   |   ✅   |   ✅   |                                                                                                                                                                          |
| Hired Ops                                                |           ✅           |   ❌   |   ❌   |   ❌   |                                                                                                                                                                          |
| League of Legends                                        |           ✅           |   ✅   |   ✅   |   ❌   |                                                                                                                                                                          |
| Middle-earth: Shadow of War                              |           ✅           |   ❌   |   ❌   |   ❌   |                                                                                                                                                                          |
| Minecraft                                                |           ✅           |   ✅   |   ✅   |   ❌   |                                                                                                                                                                          |
| Monster Hunter: World                                    |           ✅           |   ✅   |   ❌   |   ❌   |                                                                                                                                                                          |
| Monster Hunter: Rise                                     |           ✅           |   ✅   |   ❌   |   ❌   |                                                                                                                                                                          |
| Mount & Blade II: Bannerlord                             |           ✅           |   ❌   |   ❌   |   ❌   |                                                                                                                                                                          |
| Prison Architect                                         |           ✅           |   ❌   |   ❌   |   ❌   |                                                                                                                                                                          |
| Ready Or Not                                             |           ✅           |   ✅   |   ✅   |   ✅   |                                                                                                                                                                          |
| Satisfactory                                             |           ✅           |   ✅   |   ✅   |   ✅   |                                                                                                                                                                          |
| Star Wars Jedi: Fallen Order                             |           ✅           |   ❌   |   ❌   |   ❌   |                                                                                                                                                                          |
| The Ascent                                               |           ✅           |   ✅   |   ❌   |   ❌   |                                                                                                                                                                          |
| The Elder Scrolls V: Skyrim (Original + Special Edition) |           ✅           |   ❌   |   ❌   |   ❌   |                                                                                                                                                                          |
| Train Sim World 3                                        |           ✅           |   ✅   |   ✅   |   ✅   |                                                                                                                                                                          |
| Red Dead Redemption 2                                    |           ✅           |   ✅   |   ✅   |   ✅   |                                                                                                                                                                          |
| Star Citizen                                             |           ✅           |   ❌   |   ❌   |   ❌   |                                                                                                                                                                          |
| The Witcher 3                                            |           ✅           |   ✅   |   ✅   |   ✅   |                                                                                                                                                                          |
| Universe Sandbox                                         |           ✅           |   ❌   |   ❌   |   ❌   |                                                                                                                                                                          |
| Warframe                                                 |           ✅           |   ❌   |   ❌   |   ❌   |                                                                                                                                                                          |
| World of Warcraft                                        |           ✅           |   ✅   |   ✅   |   ✅   |                                                                                                                                                                          |
| X4: Foundations                                          |           ✅           |   ❌   |   ❌   |   ❌   |                                                                                                                                                                          |
