<p align="center">
  <a href="#">
    <img alt="Exatek" src="https://cdn.exatek.de/exatek/exa-partner.png" width="300" />
  </a>
</p>

# ExaGameBooster™
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/Exatek-Germany/ExaGameBooster)](https://github.com/Exatek-Germany/ExaGameBooster/releases)
[![Github All Releases](https://img.shields.io/github/downloads/Exatek-Germany/ExaGameBooster/total.svg)](https://github.com/Exatek-Germany/ExaGameBooster/releases)
[![](https://img.shields.io/static/v1?label=Games&message=2057&color=success)](https://github.com/Exatek-Germany/ExaGameBooster/releases)
[![](https://img.shields.io/static/v1?label=platform&message=win-64&color=lightgrey)](https://github.com/Exatek-Germany/ExaGameBooster/releases)
[![](https://img.shields.io/static/v1?label=dependencies&message=0&color=success)](https://github.com/Exatek-Germany/ExaGameBooster/releases)
[![](https://img.shields.io/discord/1082631532419743804)](https://discord.gg/PsR7cbWaPNhttps://discord.gg/PsR7cbWaPN)



ExaGameBooster™ is a open-source process optimization service for games. This program optimizes core allocations
for games to improve performance and reduce lags. We support modern CPU architectures such as AMD 3D V-Cache™ and Intel
P+E cores.

This software is provided "as is" without warranty of any kind, either express or implied. Use at your own risk.

### Roadmap (as of 2023-03-07)

- v2.4.\*: V-Cache support for more games (March)
- v3.0.0: Improved Installer, New Uninstaller and Updater + V-Cache support for more games (April/May)

### How to install

Simply download and open the ExaGameBooster_64.zip archive. Run the install.bat (administrator rights required) to install the executable and set up the autostart. You can also just run ExaGameBooster.exe to test without installation/autostart.

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

Since version 2.3.0 we support ~2050 games. BattlEye and VAC games are now also supported. Please note that Easy Anti Cheat games are not supported because they block CPU affinity settings.
