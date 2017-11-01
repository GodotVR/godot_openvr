extends ARVRController

var ovr_render_model

func _ready():
	# instance our render model object
	ovr_render_model = preload("res://bin/OpenVRRenderModel.gdns").new()
	
	# hide to begin with
	visible = false

func _process(delta):
	if !get_is_active():
		visible = false
	else:
		if !visible:
			# became active? lets handle it...
			var name = get_controller_name()
			print("Controller " + name + " became active")
			
			# attempt to load a mesh for this
			name = name.substr(0, name.length()-2)
			var loaded = ovr_render_model.load_model(name)
			if !loaded:
				loaded = ovr_render_model.load_model("generic_controller")
			
			if loaded:
				get_node("Controller_mesh").mesh = ovr_render_model
			else:
				get_node("Controller_mesh").mesh = Mesh()

			# make it visible
			visible = true
		
		# lets make our axis visible
		$Axis_0.scale = Vector3(1.0, 1.0, abs(get_joystick_axis(0)))
		$Axis_0.translation = Vector3(-0.07, 0.0, -0.05 + (-0.05 * get_joystick_axis(0)))
		$Axis_1.scale = Vector3(1.0, 1.0, abs(get_joystick_axis(1)))
		$Axis_1.translation = Vector3(-0.05, 0.0, -0.05 + (-0.05 * get_joystick_axis(1)))
		$Axis_2.scale = Vector3(1.0, 1.0, abs(get_joystick_axis(2)))
		$Axis_2.translation = Vector3(-0.03, 0.0, -0.05 + (-0.05 * get_joystick_axis(2)))
		$Axis_3.scale = Vector3(1.0, 1.0, abs(get_joystick_axis(3)))
		$Axis_3.translation = Vector3(-0.01, 0.0, -0.05 + (-0.05 * get_joystick_axis(3)))
		$Axis_4.scale = Vector3(1.0, 1.0, abs(get_joystick_axis(4)))
		$Axis_4.translation = Vector3(0.01, 0.0, -0.05 + (-0.05 * get_joystick_axis(4)))
		$Axis_5.scale = Vector3(1.0, 1.0, abs(get_joystick_axis(5)))
		$Axis_5.translation = Vector3(0.03, 0.0, -0.05 + (-0.05 * get_joystick_axis(5)))
		$Axis_6.scale = Vector3(1.0, 1.0, abs(get_joystick_axis(6)))
		$Axis_6.translation = Vector3(0.05, 0.0, -0.05 + (-0.05 * get_joystick_axis(6)))
		$Axis_7.scale = Vector3(1.0, 1.0, abs(get_joystick_axis(7)))
		$Axis_7.translation = Vector3(0.07, 0.0, -0.05 + (-0.05 * get_joystick_axis(7)))
		
		# make our buttons visible
		$Button_0.visible = is_button_pressed(0)
		$Button_1.visible = is_button_pressed(1)
		$Button_2.visible = is_button_pressed(2)
		$Button_3.visible = is_button_pressed(3)
		$Button_4.visible = is_button_pressed(4)
		$Button_5.visible = is_button_pressed(5)
		$Button_6.visible = is_button_pressed(6)
		$Button_7.visible = is_button_pressed(7)
		
		# and test our rumble
		rumble = get_joystick_axis(2)


func _on_OVRController_button_pressed( button ):
	print("Button " + str(button) + " was pressed on controller " + get_controller_name())

func _on_OVRController_button_release( button ):
	print("Button " + str(button) + " was released on controller " + get_controller_name())
