extends Spatial

func _process(delta):
	# Test for escape to close application, space to reset our reference frame
	if (Input.is_key_pressed(KEY_ESCAPE)):
		get_tree().quit()
	elif (Input.is_key_pressed(KEY_SPACE)):
		# Calling center_on_hmd will cause the ARVRServer to adjust all tracking data so the player is centered on the origin point looking forward
		ARVRServer.center_on_hmd(true, true)


# Called when the node enters the scene tree for the first time.
func _ready():
	# at the moment we can't seem to extend a gdnative class so we'll do this here
	$Player/Right_Hand/Viewport2Din3D.get_scene_instance().set_controller($Player/Right_Hand)

func _on_Right_Hand_action_pressed(action):
	print("Action pressed " + action)
	$Player/Right_Hand.trigger_haptic("/actions/godot/out/haptic", 1.0, 4.0, 1.0)



func _on_Toggle_Guardian_pressed():
	$Player/Guardian.visible = !$Player/Guardian.visible
