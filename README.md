# GDNative based OpenVR plugin for Godot

This is a GDNative based plugin that adds OpenVR support to Godot.

Submodules
----------
This project references two submodules. If you do not already have these repositories downloaded somewhere you can execute:
```
git submodule init
git submodule update
```
To download the required versions.

Godot_headers is a git repository that keeps a copy of the headers needed for compiling GDNative modules. It ususally contains a copy of the latest official release of Godot and may be outdated.
You can use the switch headers or set the environment variable GODOT_HEADERS to the location of more recent files. You'll need to clone a copy of the godot source code for this.

OpenVR is a git repository maintained by Valve that contains the OpenVR SDK used to interact with the OpenVR/SteamVR platform.
Alternatively you can use the switch openvr or set the environment variable OPENVR_PATH to the location where you have downloaded a copy of this SDK.

Compiling
---------
Scons is used for compiling this module. I made the assumption that scons is installed as it is also used as the build mechanism for Godot and if you are building from source you will likely need to build Godot as well.

You can compile this module by executing:
```
scons platform=windows
```

Platform can be windows, linux or osx. OSX is untested.

Note that the master in this repository is lined up with the master godot source. New functionality that is dependent on PRs not yet added to the Godot master will also have accompanying PRs here and won't be merged into master until they are ready.
There is also an alpha 2 branch that is lined up with the alpha 2 Godot release.

Deploying
---------
Note that besides compiling the GDNative module you must also include valves openvr_api.dll (windows), libopenvr_api.so (linux) or OpenVR.framework (Mac OS X).
At the moment these must be placed alongside the godot executable (on Mac OS X the framework must be placed in frameworks inside of the app bundle).
The godot_openvr.dll or libgodot_openvr.so file should be placed in the location the godot_openvr.tres file is pointing to (at the moment bin).

Note that as at the time of writing this I have only tested this on Windows. Others have let me know the linux build is working as well but tweaks may be needed.

License
-------
Note that the source in this repository is licensed by the unlicense license model. This covers only the source code in this repository. Both Godot and OpenVR have their own license requirements. See their respective git repositories for more details.
