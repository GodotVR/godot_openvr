extends CanvasLayer

var controller : XRController3D

func set_controller(new_controller: XRController3D):
	controller = new_controller

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta):
	if controller:
		var primary = controller.get_vector2("primary")
		$VBoxContainer/PrimaryJoyX/Value.value = 100.0 * primary.x
		$VBoxContainer/PrimaryJoyY/Value.value = 100.0 * primary.y
		
		var secondary = controller.get_vector2("secondary")
		$VBoxContainer/SecondaryJoyX/Value.value = 100.0 * secondary.x
		$VBoxContainer/SecondaryJoyY/Value.value = 100.0 * secondary.y

		var trigger = controller.get_float("trigger_value")
		$VBoxContainer/Trigger/Value.value = 100.0 * trigger
		$VBoxContainer/Trigger/Pressed.button_pressed = controller.is_button_pressed("trigger_click")

		var grip = controller.get_float("grip_value")
		$VBoxContainer/Grip/Value.value = 100.0 * grip
		$VBoxContainer/Grip/Pressed.button_pressed = controller.is_button_pressed("grip_click")
		$VBoxContainer/AX/Pressed.button_pressed = controller.is_button_pressed("ax")
		$VBoxContainer/BY/Pressed.button_pressed = controller.is_button_pressed("by")
		
