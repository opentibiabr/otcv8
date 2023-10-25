# OTClientV8
[![Discord Shield](https://discordapp.com/api/guilds/528117503952551936/widget.png?style=shield)](https://discord.gg/gvTj5sh9Mp)
[![Build - Android](https://github.com/opentibiabr/otcv8/actions/workflows/build-android.yml/badge.svg)](https://github.com/opentibiabr/otcv8/actions/workflows/build-android.yml)
[![Build - Ubuntu](https://github.com/opentibiabr/otcv8/actions/workflows/build-ubuntu.yml/badge.svg)](https://github.com/opentibiabr/otcv8/actions/workflows/build-ubuntu.yml)
[![Build - Windows](https://github.com/opentibiabr/otcv8/actions/workflows/build-windows.yml/badge.svg)](https://github.com/opentibiabr/otcv8/actions/workflows/build-windows.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Supported platforms:
- Windows (min. Windows 7)
- Android (min. 5.0)
- Linux
- Mac Os (requires https://www.xquartz.org/)

### Based on [otcv8/otcv8-dev](https://github.com/otcv8/otcv8-dev) Rev: [229](https://github.com/OTCv8/otcv8-dev/commit/3d32139512cc4576b105682c3579f18fe0d534e4)

### Features

- Rewritten and optimized rendering (60 fps on 11 years old computer)
- Better DirectX9 and DirectX11 support
- Adaptive rendering (automated graphics optimizations)
- Rewritten and optimized light rendering
- Rewritten path finding and auto walking
- Rewritten walking system with animations
- HTTP/HTTPS lua API with JSON support
- WebSocket lua API
- Auto updater with failsafe (recovery) mode
- New filesystem
- File encryption and compression
- Automatic diagnostic system
- Refreshed interface
- New crash and error handler
- New HTTP login protocol
- Ingame shop
- Updated hotkey manager
- Updated and optimized battle list
- Crosshair, floor fading, extra health/mana bars and panels
- Much more client options
- Removed a lot of useless and outdated things
- Advanced bot
- Linux version
- Full tibia 11.00 support
- Layouts
- New login server (with ingame account and character creation)
- Support for proxies to lower latency and protect against DDoS
- And hundreds of smaller features, optimizations and bug fixes!

### What is otclient?

Otclient is an alternative Tibia client for usage with otserv. It aims to be complete and flexible,
for that it uses LUA scripting for all game interface functionality and configurations files with a syntax
similar to CSS for the client interface design. Otclient works with a modular system, this means
that each functionality is a separated module, giving the possibility to users modify and customize
anything easily. Users can also create new mods and extend game interface for their own purposes.

For a server to connect to, you can build your own with [canary](https://github.com/opentibiabr/canary).

### Getting **Started**

* [Gitbook](https://docs.opentibiabr.com/opentibiabr/projects/otclientv8).

### Need help?

Try to ask questions in [discord](https://discord.gg/gvTj5sh9Mp).

### Issues

We use the [issue tracker on GitHub](https://github.com/opentibiabr/otcv8/issues). Keep in mind that everyone who is watching the repository gets notified by e-mail when there is an activity, so be thoughtful and avoid writing comments that aren't meant for an issue (e.g. "+1"). If you'd like for an issue to be fixed faster, you should either fix it yourself and submit a pull request, or place a bounty on the issue.

### Pull requests

Before [creating a pull request](https://github.com/opentibiabr/otcv8/pulls) please keep in mind:

  * Focus on fixing only one thing, mixing too much things on the same Pull Request make it harder to review, harder to test and if we need to revert the change it will remove other things together.
  * Follow the project indentation, if your editor support you can use the [editorconfig](https://editorconfig.org/) to automatic configure the indentation.
  * Avoid opening a Pull Request to just update one line of an file.

### Credits

  * [kondra](https://github.com/OTCv8)
  * [oen432](https://github.com/Oen44)
  * [vithrax](https://github.com/Vithrax)
  * [our contributors](https://github.com/opentibiabr/otcv8/graphs/contributors)

### License

Otclient is made available under the MIT License, thus this means that you are free
to do whatever you want, commercial, non-commercial, closed or open.
