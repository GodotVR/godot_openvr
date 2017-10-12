# GDNative based OpenVR plugin for Godot

This is a GDNative based plugin that adds OpenVR support to Godot.
In order to use this you need to have a Godot build that includes the work in this PR:
https://github.com/godotengine/godot/pull/11937

Note that besides compiling the GDNative module you must also include valves openvr_api.dll.
At the moment this file must be placed alongside the godot.exe.
The godot_openvr.dll file should be placed in the location the godot_openvr.tres file is pointing to (at the moment demo/bin).

More info to follow soon. This is still a work in progress...