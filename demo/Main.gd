extends Spatial

func _ready():
	# Find the interface and initialise
	var arvr_interface = ARVRServer.find_interface("OpenVR")
	if arvr_interface and arvr_interface.initialize():
		get_viewport().arvr = true

		# workaround in OpenVR because OpenVR does not like our HDR buffers, so turn it off for now...
		get_viewport().hdr = false
		
		# resize our window so we see a smaller preview of our left eye
		var size = arvr_interface.get_recommended_render_targetsize() / 3.0
		OS.set_window_size(size);

func _process(delta):
	# Test for escape to close application, space to reset our reference frame
	if (Input.is_key_pressed(KEY_ESCAPE)):
		get_tree().quit()
	elif (Input.is_key_pressed(KEY_SPACE)):
		# Calling center_on_hmd will cause the ARVRServer to adjust all tracking data so the player is centered on the origin point looking forward
		ARVRServer.center_on_hmd(true, true)

	# We minipulate our origin point to move around. Note that with roomscale tracking a little more then this is needed
	# because we'll rotate around our origin point, not around our player. But that is a subject for another day.
	if (Input.is_key_pressed(KEY_LEFT)):
		$ARVROrigin.rotation.y += delta
	elif (Input.is_key_pressed(KEY_RIGHT)):
		$ARVROrigin.rotation.y -= delta

	if (Input.is_key_pressed(KEY_UP)):
		$ARVROrigin.translation -= $ARVROrigin.transform.basis.z * delta;
	elif (Input.is_key_pressed(KEY_DOWN)):
		$ARVROrigin.translation += $ARVROrigin.transform.basis.z * delta;