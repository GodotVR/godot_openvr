tool
extends EditorPlugin

var export_plugin

func _enter_tree():
	# Initialization of the plugin goes here
	export_plugin = preload("res://addons/godot-openvr/OpenVRExportPlugin.gd")

	if export_plugin:
		# print("Loaded res://addons/godot-openvr/OpenVRExportPlugin.gd")

		export_plugin = export_plugin.new()
		if export_plugin:
			# print("New res://addons/godot-openvr/OpenVRExportPlugin.gd")
			add_export_plugin(export_plugin)
	#	else:
	#		print("Failed to instance res://addons/godot-openvr/OpenVRExportPlugin.gd")
	#else:
	#	print("Failed to load res://addons/godot-openvr/OpenVRExportPlugin.gd")

func _exit_tree():
	# Clean-up of the plugin goes here
	if export_plugin:
		remove_export_plugin(export_plugin)
