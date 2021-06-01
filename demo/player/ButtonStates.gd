extends CanvasLayer

var controller : XRController3D

func set_controller(new_controller: XRController3D):
	controller = new_controller

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if controller:
		$VBoxContainer/JoyX/Value.value = 100.0 * controller.get_joystick_axis(0)
		$VBoxContainer/JoyY/Value.value = 100.0 * controller.get_joystick_axis(1)
		$VBoxContainer/Trigger/Value.value = 100.0 * controller.get_joystick_axis(2)
		$VBoxContainer/Trigger/Pressed.pressed = controller.is_button_pressed(15)
		$VBoxContainer/Grip/Value.value = 100.0 * controller.get_joystick_axis(4)
		$VBoxContainer/Grip/Pressed.pressed = controller.is_button_pressed(2)
		$VBoxContainer/AX/Pressed.pressed = controller.is_button_pressed(7)
		$VBoxContainer/BY/Pressed.pressed = controller.is_button_pressed(1)
		
