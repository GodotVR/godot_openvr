extends ARVRController

var ovr_render_model
var components = Array()

func _ready():
	# instance our render model object
	ovr_render_model = preload("res://bin/OpenVRRenderModel.gdns").new()
	
	# hide to begin with
	visible = false

func clear_meshes():
	var meshes = get_node("Controller_meshes")
	while meshes.get_child_count() > 0:
		var child = meshes.get_child(0)
		meshes.remove_child(child)
	components = Array()

func add_mesh(p_name, p_model):
	if (p_model != ''):
		print("Loading " + p_name + " (" + p_model +")")

		var mesh = preload("res://bin/OpenVRRenderModel.gdns").new()
		if mesh.load_model(p_model):
			var ws = ARVRServer.world_scale
			var instance = MeshInstance.new()
			instance.set_name(p_name)
			instance.mesh = mesh
			instance.scale = Vector3(ws, ws, ws)
			$Controller_meshes.add_child(instance)
		
			return true
		else:
			return false
	else:
		print("Skipping " + p_name)
		return true;

func _process(delta):
	if !get_is_active():
		if visible:
			visible = false
			clear_meshes()
	else:
		if !visible:
			# became active? lets handle it...
			var name = get_controller_name()
			print("Controller " + name + " became active")
			
			# attempt to load a mesh(es) for this
			name = name.substr(0, name.length()-2)
			components = ovr_render_model.model_list_components(name)
			if components.size() > 0:
				print("Found: " + str(components))
				# yeah! we have components, lets load those components
				for component in components:
					add_mesh(component["component_name"], component["render_model_name"])
			else:
				var loaded = add_mesh(name, name)
				if !loaded:
					loaded = add_mesh("generic_controller", "generic_controller")

			# make it visible
			visible = true
		
		# lets make our axis visible
		$Axis_0.scale = Vector3(1.0, 1.0, abs(get_joystick_axis(0)))
		$Axis_0.translation = Vector3(-0.09, 0.0, -0.05 + (-0.05 * get_joystick_axis(0)))
		$Axis_1.scale = Vector3(1.0, 1.0, abs(get_joystick_axis(1)))
		$Axis_1.translation = Vector3(-0.07, 0.0, -0.05 + (-0.05 * get_joystick_axis(1)))
		$Axis_2.scale = Vector3(1.0, 1.0, abs(get_joystick_axis(2)))
		$Axis_2.translation = Vector3(-0.05, 0.0, -0.05 + (-0.05 * get_joystick_axis(2)))
		$Axis_3.scale = Vector3(1.0, 1.0, abs(get_joystick_axis(3)))
		$Axis_3.translation = Vector3(-0.03, 0.0, -0.05 + (-0.05 * get_joystick_axis(3)))
		$Axis_4.scale = Vector3(1.0, 1.0, abs(get_joystick_axis(4)))
		$Axis_4.translation = Vector3(-0.01, 0.0, -0.05 + (-0.05 * get_joystick_axis(4)))
		$Axis_5.scale = Vector3(1.0, 1.0, abs(get_joystick_axis(5)))
		$Axis_5.translation = Vector3(0.01, 0.0, -0.05 + (-0.05 * get_joystick_axis(5)))
		$Axis_6.scale = Vector3(1.0, 1.0, abs(get_joystick_axis(6)))
		$Axis_6.translation = Vector3(0.03, 0.0, -0.05 + (-0.05 * get_joystick_axis(6)))
		$Axis_7.scale = Vector3(1.0, 1.0, abs(get_joystick_axis(7)))
		$Axis_7.translation = Vector3(0.05, 0.0, -0.05 + (-0.05 * get_joystick_axis(7)))
		$Axis_8.scale = Vector3(1.0, 1.0, abs(get_joystick_axis(8)))
		$Axis_8.translation = Vector3(0.07, 0.0, -0.05 + (-0.05 * get_joystick_axis(8)))
		$Axis_9.scale = Vector3(1.0, 1.0, abs(get_joystick_axis(9)))
		$Axis_9.translation = Vector3(0.09, 0.0, -0.05 + (-0.05 * get_joystick_axis(9)))
		
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
