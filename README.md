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
Note that besides compiling the GDNative module you must also include valves openvr_api.dll (windows), libopenvr_api.so (linux) or OpenVR.framework (Mac OS X). See platform notes for placement of these files.
The godot_openvr.dll or libgodot_openvr.so file should be placed in the location the godot_openvr.gdnlib file is pointing to (at the moment bin).

Note that as at the time of writing this I have only tested this on Windows. Others have let me know the Linux build is working as well but tweaks may be needed.

Mac notes
---------
Mac is currently untested, I unfortunately do not have the required hardware. If anyone wants to hold up their hands, please contact me :)

Linux notes
-----------
On Linux, Steam will not automatically add the SteamVR libraries to your $LD_LIBRARY_PATH variable. As such, when starting Godot (for development) or your game outside of Steam, it will fail to load the required libraries.

There are a couple of ways to fix this:

1) Launch Godot or your game from within Steam

2) Run Godot or your game through the steam runtime manually (change the path to suit your Steam installation):

```
/home/<user>/.steam/steam/ubuntu12_32/steam-runtime/run.sh <your command>
```

3) Adjust your $LD_LIBRARY_PATH variable accordingly:

```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/path/to/libgodot_openvr.so/dir/":"/home/<user>/.steam/steam/steamapps/common/SteamVR/bin/"
```

You can place the libopenvr_api.so file alongside the libgodot_openvr.so file in the bin folder. You can find this file in: openvr/bin/linux64

Windows notes
-------------

Windows generally works without any special needs. I've tested compiling with MSVC 2017, others have tested 2015. With 2017 be aware Microsoft now lets you pick and choose which components you install and by default it may not install the SDK you need. Make sure to install both the Windows SDK and build tools.

Also when deploying users may need to first install the correct redistributable you can find here: https://support.microsoft.com/en-au/help/2977003/the-latest-supported-visual-c-downloads
I am not 100% sure this is a requirement as it automatically installs this when installing MSVC but past experiences and such... :)

For Windows you need to supply a copy of openvr_api.dll along with your executable which can be found in openvr/bin/win64
Up until and including Godot 3 beta 0 this file needs to be placed alongside the EXE.
If you build Godot from the latest master you can place the file in the bin folder alongside the godot_openvr.dll

License
-------
Note that the source in this repository is licensed by the unlicense license model. This covers only the source code in this repository. Both Godot and OpenVR have their own license requirements. See their respective git repositories for more details.
