extends ARVRController

func _ready():
	set_process(true)

func _process(delta):
	var rot = get_node("Trigger_origin").get_rotation_deg()
	rot.x = get_joystick_axis(1) * -30.0
	get_node("Trigger_origin").set_rotation_deg(rot)
	
	# only show active controllers
	visible = get_is_active()