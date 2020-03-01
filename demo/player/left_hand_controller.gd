extends "res://addons/godot-openvr/scenes/ovr_controller.gd"

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	# update our rumble
	if is_button_pressed(JOY_VR_TRIGGER):
		rumble = 1.0
	else:
		rumble = 0.0
