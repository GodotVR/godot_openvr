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


func _on_OVRController_button_pressed( button ):
	print("Button " + str(button) + " was pressed on controller " + get_controller_name())

func _on_OVRController_button_release( button ):
	print("Button " + str(button) + " was released on controller " + get_controller_name())
