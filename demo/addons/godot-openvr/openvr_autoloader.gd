@tool
extends Node

var _xr_interface_openvr : XRInterfaceOpenVR

func get_interface():
	return _xr_interface_openvr

func start_xr():
	if _xr_interface_openvr:
		print("Capabilities " + str(_xr_interface_openvr.get_capabilities()))
		print("Target size " + str(_xr_interface_openvr.get_render_target_size()))

		if _xr_interface_openvr.initialize():
			get_viewport().use_xr = true

			print("Initialised")
		else:
			print("Failed to initialise")
	else:
		print("Interface was not instantiated")


func _enter_tree():
	_xr_interface_openvr = XRInterfaceOpenVR.new()
	if _xr_interface_openvr:
		XRServer.add_interface(_xr_interface_openvr)


func _exit_tree():
	if _xr_interface_openvr:
		XRServer.remove_interface(_xr_interface_openvr)
		_xr_interface_openvr = null
