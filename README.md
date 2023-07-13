# OTCv8 Developer Editon (sources)

Ready to use binaries are available in the [Binary Files Release](https://github.com/opentibiabr/otcv8/releases/tag/binary-files) repository.

OTCv8 sources. You can add whatever you want and create pull request with your changes.
Accepted pull requests will be added to official OTCv8 version, so if you want a new feature in OTCv8, just add it here and wait for approval.
If you add custom feature, make sure it's optional and can be enabled via g_game.enableFeature function, otherwise your pull request will be rejected.

This repository uses Github Actions to build and test OTCv8 automaticlly whenever you push changes to repository.

## Compilation

### Automatic

You can clone repoistory and use github action build-on-request workload.

### Windows

You need visual studio 2019 and vcpkg with commit `3b3bd424827a1f7f4813216f6b32b6c61e386b2e` ([download](https://github.com/microsoft/vcpkg/archive/3b3bd424827a1f7f4813216f6b32b6c61e386b2e.zip)).

Then you install vcpkg dependencies:
```bash
vcpkg install boost-iostreams:x86-windows-static boost-asio:x86-windows-static boost-beast:x86-windows-static boost-system:x86-windows-static boost-variant:x86-windows-static boost-lockfree:x86-windows-static boost-process:x86-windows-static boost-program-options:x86-windows-static luajit:x86-windows-static glew:x86-windows-static boost-filesystem:x86-windows-static boost-uuid:x86-windows-static physfs:x86-windows-static openal-soft:x86-windows-static libogg:x86-windows-static libvorbis:x86-windows-static zlib:x86-windows-static libzip:x86-windows-static openssl:x86-windows-static
```

and then you can compile static otcv8 version.

### Linux

on linux you need:
- vcpkg from commit `761c81d43335a5d5ccc2ec8ad90bd7e2cbba734e`
- boost >=1.67 and libzip-dev, physfs >= 3
- gcc >=9

Then just run mkdir build && cd build && cmake .. && make -j8

### Android

To compile on android you need to create C:\android with
- [android-ndk-r21b](https://dl.google.com/android/repository/android-ndk-r21d-windows-x86_64.zip)
- [android-25](https://dl.google.com/android/repository/platform-25_r03.zip)
- [platform-tools r34.0.1](https://dl.google.com/android/repository/platform-tools_r34.0.1-windows.zip)
- [tools r25.2.5](https://dl.google.com/android/repository/tools_r25.2.5-windows.zip)
- [android_libs.7z](https://github.com/opentibiabr/otcv8/releases/download/binary-files/android_libs.7z)

Also install android extension for visual studio
In visual studio go to options -> cross platform -> c++ and set Android NDK to C:\android\android-ndk-r21b
Right click on otclientv8 -> proporties -> general and change target api level to android-25

#### Follow the same step for the others downloads

![image](https://github.com/opentibiabr/otcv8/assets/8551443/117f18e6-d77c-4e73-bf42-3bb2e9c9968b)

Put data.zip in android/otclientv8/assets

## Links

- Discord: https://discord.gg/gvTj5sh9Mp
- Forum: https://docs.opentibiabr.com/
- Email: opentibiabr@outlook.com
