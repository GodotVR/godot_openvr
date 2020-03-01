extends CanvasLayer

var controller : ARVRController

func set_controller(new_controller: ARVRController):
	controller = new_controller

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if controller:
		$VBoxContainer/JoyX/Value.value = 100.0 * controller.get_joystick_axis(JOY_OPENVR_TOUCHPADX)
		$VBoxContainer/JoyY/Value.value = 100.0 * controller.get_joystick_axis(JOY_OPENVR_TOUCHPADY)
		$VBoxContainer/Trigger/Value.value = 100.0 * controller.get_joystick_axis(JOY_VR_ANALOG_TRIGGER)
		$VBoxContainer/Trigger/Pressed.pressed = controller.is_button_pressed(JOY_VR_TRIGGER)
		$VBoxContainer/Grip/Value.value = 100.0 * controller.get_joystick_axis(JOY_VR_ANALOG_GRIP)
		$VBoxContainer/Grip/Pressed.pressed = controller.is_button_pressed(JOY_VR_GRIP)
		$VBoxContainer/AX/Pressed.pressed = controller.is_button_pressed(7)
		$VBoxContainer/BY/Pressed.pressed = controller.is_button_pressed(1)
		
