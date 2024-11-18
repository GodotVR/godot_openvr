# GDExtension based OpenVR plugin for Godot

This is a GDExtension based plugin that adds OpenVR support to Godot.

The leading version of this repository now lives at:
https://github.com/GodotVR/godot_openvr

>> **Important** this version of the OpenVR plugin now uses the OpenVR actions system.
>> While we have taken care to try and keep a measure of backwards compatibility there are structural differences in how OpenVR handles actions and once enabled the old method of button and axis handling is deactivated.
>> Please read the [OpenVR Actions documentation](https://github.com/GodotVR/godot-openvr-asset/wiki/OpenVR-actions) for more information!

# Branches
This repo currently maintains the following branches:
- `master` is our main development in which we maintain our `1.x.y` version of this plugin. This version works with Godot 3.x.
- `2.0-dev` is our 2.0 development branch, this is an unstable branch in which we're porting this plugin to work in Godot 4.x.

# Building

## Submodules
After cloning this repo, you should run:

	git submodule update --init --recursive

We use the following submodules:

* [godot-cpp](https://github.com/godotengine/godot-cpp): C++ bindings to Godot's internal classes. This has a submodule of its own, godot-headers.
* [OpenVR](https://github.com/ValveSoftware/openvr): the repository maintained by Valve that contains the OpenVR SDK used to interact with the OpenVR/SteamVR platform.

The godot-cpp module is pointed at the branch corresponding to the specific release of Godot that we're currently supporting. 2.x of this extension has only been tested with OpenVR 2.0+.

OpenVR can also be stored elsewhere using the `openvr_path=` build argument, or discovered automatically from the system if you're using a mingw/linux build environment. See Build variables below.

## Compiling
SCons is used for compiling this module. This is also the build mechanism for Godot and information on installing it on various platforms can be found in [the Godot documentation](https://docs.godotengine.org/en/stable/contributing/development/compiling/introduction_to_the_buildsystem.html).

You must compile the `godot-cpp` submodule first by executing:

	cd godot-cpp
	scons platform=windows target=template_release generate_bindings=yes arch=x86_64
	cd ..

You can then compile this extension by executing:

	scons

The results will be placed within the extension's addon hierarchy in the included demo. This entire hierarchy (demo/addons/godot-openvr on downward) is then distributable.

### Build variables
There are many CLI flags that can be given to scons to manipulate how the extension is created:

* `target`: `release` or `debug`. Controls whether debug symbols are built into the extension and also sets appropriate compiler optimization flags.
* `platform`: `windows` or `linux`. Controls whether we build a DLL or shared library.
* `target_path`: The path where the native library for the extension is placed after building, defaults to `demo/addons/godot-openvr/bin/`.
* `target_name`: Name of the native library, defaults to `libgodot_openvr`,
* `use_mingw`: Use the Mingw compiler, even if the Visual Studio compiler is available (mostly useful on Windows).
* `use_llvm`: Use the LLVM compiler (clang).
* `use_static_cpp`: Link MinGW/MSVC C++ runtime libraries statically. Defaults to `yes` to simplify distribution and match godot-cpp.
* `bits`: `32` or `64`. Target CPU architecture, defaults to 64.
* `builtin_openvr`: Use the OpenVR library from our submodule. Defaults to `yes`, see below for how to use a system OpenVR.
* `openvr_path`: The path where the OpenVR repo is located, if `builtin_openvr` is used. Defaults to `openvr/`.

Setting `builtin_openvr=no` will cause pkg-config to be invoked to search for a `libopenvr_api` installed on your system. pkg-config normally only exists on Linux systems, but can be installed on Windows using the mingw toolchain.

When overriding any of these flags, care should be taken to build godot-cpp with the same flags where they exist. We have attempted to make them match in most cases, though the targets are different and `bits` is instead `arch={x86_64,x86_32}`.

**In particular, when building for use in the editor you should compile godot-cpp with `target=template_debug` and this extension with `target=debug`.**

See the Windows platform-specific notes below for further info about mingw and potential pitfalls.

## Deploying
The finished GDExtension must be shipped with Valve's `openvr_api.dll` (Windows) or `libopenvr_api.so` (Linux). This is handled automatically by the build system whenever possible. The expected res:// paths of the library can be found in godot\_openvr.gdextension.

## Running the demo
To use the demo you will additionally need to install the godot-xr-tools addon, which is available in the asset library within the editor. For alternative installation methods, see [the documentation](https://godotvr.github.io/godot-xr-tools/docs/installation/).
After installing, you will need to make sure it is enabled and then restart the editor.

If you are building yourself, the demo should now be runnable. If you want to test it out without building, you can download the package from the latest Github actions build, unzip it, and replace the addons/godot-openvr directory with the one from the zip. Note that the debug build is not currently included in the package.

# Platform-specific notes

## Linux
On Linux, Steam will not automatically add the SteamVR libraries to your `$LD_LIBRARY_PATH` variable. As such, when starting Godot (for development) or your game outside of Steam, it will fail to load the required libraries.

There are a couple of ways to fix this:

1) Launch Godot or your game from within Steam

2) Run Godot or your game through the steam runtime manually (change the path to suit your Steam installation):

	/home/$USER/.steam/steam/ubuntu12_32/steam-runtime/run.sh <your command>

3) Adjust your `$LD_LIBRARY_PATH` variable accordingly:

	export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/path/to/libgodot_openvr.so/dir/:/home/$USER/.steam/steam/steamapps/common/SteamVR/bin/"

## Windows
Windows generally works without any special needs. This has been compiled with MSVC 2019, and in the past has worked with 2017 and 2015. With 2017 and above be aware Microsoft now lets you pick and choose which components you install and by default it may not install the SDK you need. Make sure to install both the Windows SDK and build tools. To build using the MSVC compiler without needing to install the entire IDE, the build tools can be installed using winget:

	winget install Microsoft.VisualStudio.2019.BuildTools --override "--quiet --add Microsoft.VisualStudio.Workload.NativeDesktop"

Building with scons should then work within the Developer Powershell it creates.

Also, depending on what version of Visual Studio that was used to compile the dlls, you will need to install the `Microsoft Visual C++ Redistributable for Visual Studio` on any machine that you deploy your dll on. Godot already needs one of these but if you compile with a newer version of Visual Studio you also need to install that version. It's dumb, I know. If you build statically (`use_static_cpp`), this is not necessary as the library is compiled into the extension. Note that mixing a statically included libc++ with other versions may cause unintended side effects.
https://support.microsoft.com/en-au/help/2977003/the-latest-supported-visual-c-downloads

### mingw

Building on Windows under mingw means using the gcc c++ compiler. Because C++ does not have a stable ABI, the resulting binary cannot actually communicate successfully with SteamVR using the OpenVR C++ bindings. The fix for this is a header patch using [a script written by tunabrain](https://gist.github.com/tunabrain/1fc7a4964914d61b5ae751d0c84f2382). This script rewrites the C++ header to call the OpenVR C bindings internally, sidestepping the ABI mismatch. We do not currently attempt to run this script as part of our build, it is up to you to produce a patched header if using the submoduled libopenvr_api.

When `use_mingw` is set, `USE_OPENVR_MINGW_HEADER` is exported during the build which will cause `openvr_mingw.hpp` to be included where `openvr.h` would be normally. You may place this file anywhere you like in the header search path.

As an alternative to manually providing `openvr_mingw.hpp`, the [msys2](https://www.msys2.org/) distribution which can provide mingw-w64 also packages OpenVR along with the patched header. Setting up your mingw toolchain using msys2 (only the UCRT64 environment has been tested) and setting `builtin_openvr=no` will allow the packaged library to be discovered during the build. Setting up an msys2 environment is out of scope for this README, but once it is installed the following command will get the environment ready to build this extension:

	pacman -S mingw-w64-ucrt-x86_64-toolchain mingw-w64-ucrt-x86_64-scons mingw-w64-ucrt-x86_64-openvr

msys2 also packages Godot and godot-cpp, but as of this writing they are still on the 3.x train and thus we must continue to build our own godot-cpp.

As an aside, it would seem necessary to also build Godot itself for the mingw-compiled C++ bindings to work, but I've had no issues running the extension in the official build of Godot 4.2.

## MacOS
MacOS support in OpenVR was dropped by Valve for a long time. While it appears it has recently made a return without much fanfare in OpenVR 2.0, support has not yet been returned to our build.

# Shader hickup
There are a few moment where OpenVR has a hickup.

One is around the teleporter function which can be solved by adding the `VR_Common_Shader_Cache.tscn` as a child scene to our XRCamera3D. `ovr_first_person.tscn` does this.

For the controllers they use a standard material. Adding a mesh instance with a standard material will ensure the shader is pre-compiled. Again we do this in `ovr_first_person.tscn`.

However there is still an issue with loading the texture. We need to redo loading of the controller mesh by handing it off to a separate thread.

# Rendering

## Using the main viewport
The XR server module requires a viewport to be configured as the XR viewport. If you chose to use the main viewport an aspect ratio corrected copy of the left eye will be rendered to the viewport automatically.

You will need to add the following code to a script on your root node:

```
var interface = XRServer.find_interface("OpenVR")
if interface and interface.initialize():
	get_viewport().use_xr = true

	# make sure vsync is disabled or we'll be limited to 60fps
	DisplayServer.window_set_vsync_mode(DisplayServer.VSYNC_DISABLED)

	# up our physics to 90fps to get in sync with our rendering (you may want to query the real headset value here instead)
	Engine.iterations_per_second = 90
```

## Using a separate viewport
If you want control over the output on screen so you can show something independent on the desktop you can add a SubViewport to your scene and enable "Use XR" on it.

Also make sure that both the Clear Mode and Update Mode are set to Always, or nothing will be rendered.

You can add a normal camera to your scene to render a spectator view or turn the main viewport into a 2D viewport and save some rendering overhead.

The initialization code on the root node will now look like this:

```
var interface = XRServer.find_interface("OpenVR")
if interface and interface.initialize():
	DisplayServer.window_set_vsync_mode(DisplayServer.VSYNC_DISABLED)
	Engine.iterations_per_second = 90
```

# License
Note that the source in this repository is licensed by the MIT license model.
This covers only the source code in this repository.

Both Godot and OpenVR have their own license requirements.
See their respective git repositories for more details.

The subfolder `assets` contains third party assets.
See license files in those subfolders for additional license details

# About this repository
This repository was created by and is maintained by Bastiaan Olij a.k.a. Mux213

You can follow me on twitter for regular updates here:
https://twitter.com/mux213

Videos about my work with Godot including tutorials on working with VR in Godot can by found on my youtube page:
https://www.youtube.com/BastiaanOlij
