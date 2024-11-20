extends XRController3D

func _on_aim_right_hand_button_pressed(button_name):
	print("Button " + button_name + " pressed")
	if name =="trigger_click":
		trigger_haptic_pulse("haptic", 100.0, 10.0, 0.2, 0.0);
