extends MeshInstance

var ovr_render_model
var ws = 0
var controller_is_loaded = false

func apply_world_scale():
	var new_ws = ARVRServer.world_scale
	if (ws != new_ws):
		ws = new_ws
		scale = Vector3(ws, ws, ws)

func _load_controller_mesh(controller_name):
	if ovr_render_model.load_model(controller_name.substr(0, controller_name.length()-2)):
		return ovr_render_model
	
	if ovr_render_model.load_model("generic_controller"):
		return ovr_render_model
	
	return Mesh.new()

# Called when the node enters the scene tree for the first time.
func _ready():
	# instance our render model object
	ovr_render_model = preload("res://addons/godot-openvr/OpenVRRenderModel.gdns").new()
	
	# we haven't loaded it
	controller_is_loaded = false

func _process(delta):
	var controller = get_parent();
	if !controller is ARVRController:
		return
	
	if !controller.get_is_active():
		if controller_is_loaded:
			mesh = Mesh.new()
			controller_is_loaded = false
		return
	
	# always set our world scale, user may end up changing this
	apply_world_scale()
	
	if controller_is_loaded:
		return
	
	# became active? lets handle it...
	var controller_name = controller.get_controller_name()
	print("Controller " + controller_name + " became active")
	
	# attempt to load a mesh for this
	mesh = _load_controller_mesh(controller_name)
	controller_is_loaded = true
