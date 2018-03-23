extends Spatial

func _ready():
	# Find the interface and initialise
	var arvr_interface = ARVRServer.find_interface("OpenVR")
	if arvr_interface and arvr_interface.initialize():
		get_viewport().arvr = true
		
		# workaround in OpenVR because OpenVR does not like our HDR buffers, so turn it off for now...
		get_viewport().hdr = false
	
	# just for testing, list what models are available
	var ovr_model = preload("res://addons/godot-openvr/OpenVRRenderModel.gdns").new()
	var model_names = ovr_model.model_names()
	print("models: " + str(model_names))

func _process(delta):
	# Test for escape to close application, space to reset our reference frame
	if (Input.is_key_pressed(KEY_ESCAPE)):
		get_tree().quit()
	elif (Input.is_key_pressed(KEY_SPACE)):
		# Calling center_on_hmd will cause the ARVRServer to adjust all tracking data so the player is centered on the origin point looking forward
		ARVRServer.center_on_hmd(true, true)
