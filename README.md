<p align="center">
  <a href="#">
    <img alt="Exatek" src="https://cdn.exatek.de/exatek/exa-partner.png" width="300" />
  </a>
</p>

# ExaGameBooster™
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/Exatek-Germany/ExaGameBooster)](https://github.com/Exatek-Germany/ExaGameBooster/releases)
[![Github All Releases](https://img.shields.io/github/downloads/Exatek-Germany/ExaGameBooster/total.svg)](https://github.com/Exatek-Germany/ExaGameBooster/releases)
[![](https://img.shields.io/static/v1?label=Games&message=7606&color=success)](https://github.com/Exatek-Germany/ExaGameBooster/releases)
[![](https://img.shields.io/static/v1?label=platform&message=win-64&color=lightgrey)](https://github.com/Exatek-Germany/ExaGameBooster/releases)
[![](https://img.shields.io/static/v1?label=dependencies&message=0&color=success)](https://github.com/Exatek-Germany/ExaGameBooster/releases)
[![](https://img.shields.io/discord/1082631532419743804)](https://discord.gg/PsR7cbWaPN)



ExaGameBooster™ is a open-source process optimization service for games. This program optimizes core allocations
for games to improve performance and reduce lags. We support modern CPU architectures such as AMD 3D V-Cache™ and Intel
P+E cores.

This software is provided "as is" without warranty of any kind, either express or implied. Use at your own risk.

### Roadmap (as of 2024-03-21)

- v3.1: Support for new Zen5 products (12/16 cores)
- v3.1+: Support for more games (including V-Cache optimizations)

### Survey on support for the 14th generation of Intel and future Intel products

Please vote if you would like us to add support: https://strawpoll.com/poy9W3aBPgJ

### How to install

Simply download and run the Installer.exe executable (administrator privileges required). This will install the service and set up the autostart. You can also just run ExaGameBooster.exe to test without installation/autostart.

### Supported CPU's

> **_NOTE:_**  If your CPU is not in the list, you probably do not need this program. This program solves scheduler
> problems caused by multi-die CPUs.

#### AMD Ryzen Raphael (7000 Series)

| Version | Ryzen 9 7950X3D* | Ryzen 9 7900X3D* | Ryzen 9 7950X | Ryzen 9 7900X | Ryzen 9 7900 |
|---------|:---------------:|:---------------:|:-------------:|:-------------:|:------------:|
| 2.0.0   |        ✅        |        ✅        |       ✅       |       ✅       |      ✅       |
| 1.1.0   |        ✅        |        ✅        |       ✅       |       ✅       |      ❌       |
| 1.0.0   |        ✅        |        ❌        |       ✅       |       ❌       |      ❌       |

\* we recommend uninstalling the "AMD 3D V-Cache Performance Optimizer Driver" or enabling the "high performance" power plan to avoid scheduling problems

#### AMD Ryzen Dragon Range (7000 Mobile Series)

| Version | Ryzen 9 7945HX3D* | Ryzen 9 7945HX |
|---------|:---------------:|:---------------:|
| 3.0.0   |        ✅        |        ✅        |
| 2.0.0   |        ❌        |        ❌        |
| 1.1.0   |        ❌        |        ❌        |
| 1.0.0   |        ❌        |        ❌        |

\* we recommend uninstalling the "AMD 3D V-Cache Performance Optimizer Driver" or enabling the "high performance" power plan to avoid scheduling problems

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

| Version | i9 13900 | i7 13700 | i5 13600 | i5 13500 | i5 13400 | Mobile CPUs (4+ P-Cores) |
|---------|:--------:|:--------:|:--------:|:--------:|:--------:|:------------------------:|
| 2.1.0   |    ✅     |    ✅     |    ✅     |    ✅     |    ✅     |            ✅             | 
| 1.0.0   |    ✅     |    ✅     |    ✅     |    ✅     |    ✅     |            ❌             | 

#### Intel Alder Lake (12th Gen)

| Version | i9 12900 | i7 12700 | i5 12600K | Mobile CPUs (4+ P-Cores) |
|---------|:--------:|:--------:|:---------:|:------------------------:|
| 2.1.0   |    ✅     |    ✅     |     ✅     |            ✅             |
| 1.0.0   |    ✅     |    ✅     |     ✅     |            ❌             |

### Supported Operating Systems

ExaGameBooster™ is supported by Windows 10 and 11. We currently do not plan to extend the supported operating
systems.

### Supported Games

> **_NOTE:_**  We are constantly working to test and add new games. If you would like to see support for specific games,
> please create an issue on GitHub or email us at gamebooster@exatek.de.

Since version v3.0.1 we support ~7600 games. BattlEye and VAC games are now also supported. Please note that some Easy Anti Cheat games are not supported because they block CPU affinity settings.

> **_NOTE:_**  We will wait to add CS2 support until we can test anti-cheat compatibility after the full release and check back with Valve to see if any changes in VAC could affect ExaGameBooster compatibility. This is done to avoid the risk of VAC bans. Please wait until we have clarified this.

You can now search the supported games on our website: [Search Games](https://exatek.de/products/exa-game-booster)