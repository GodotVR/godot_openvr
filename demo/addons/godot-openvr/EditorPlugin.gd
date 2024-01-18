@tool
extends EditorPlugin

var export_plugin

static var _initialized := false

func _openvr_init():
	if not _initialized:
		add_autoload_singleton("OpenVRInterface", "res://addons/godot-openvr/openvr_autoloader.gd")
		_initialized = true

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

	_openvr_init()

func _exit_tree():
	# Clean-up of the plugin goes here
	if export_plugin:
		remove_export_plugin(export_plugin)

func _enable_plugin():
	_openvr_init()

func _disable_plugin():
	if _initialized:
		remove_autoload_singleton("OpenVRInterface")
		_initialized = false
