extends Spatial

func _ready():
	# Find the interface and initialise
	var arvr_interface = ARVRServer.find_interface("OpenVR")
	if arvr_interface and arvr_interface.initialize():
		# switch to ARVR mode
		get_viewport().arvr = true
		
		# workaround for OpenVR not supporting RGBA16F buffers, not needed for GLES2 renderer.
		get_viewport().hdr = false
		
		# make sure vsync is disabled or we'll be limited to 60fps
		OS.vsync_enabled = false
		
		# up our physics to 90fps to get in sync with our rendering
		Engine.target_fps = 90
	
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
