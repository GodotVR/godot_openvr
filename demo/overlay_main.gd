extends Node

var _xr_interface_openvr : XRInterfaceOpenVR


func _enter_tree():
	_xr_interface_openvr = OpenVRInterface.get_interface()

	_xr_interface_openvr.set_application_type(2)
	_xr_interface_openvr.set_tracking_universe(1)

	if not _xr_interface_openvr.initialize():
		print("Failed to connect to OpenVR")

