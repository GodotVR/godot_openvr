extends MeshInstance3D

class_name OVRRenderModel3D

var ovr: XRInterfaceOpenVR

var ws = 0
var controller_is_loaded = false

## Override the render model. If unset and the parent of this node is an XRController3D,
## an attempt will be made to locate the correct model automatically.
@export var render_model_name: String:
	set(name):
		render_model_name = name
		controller_is_loaded = false

func apply_world_scale():
	var new_ws = XRServer.world_scale
	if (ws != new_ws):
		ws = new_ws
		scale = Vector3(ws, ws, ws)
#
# Called when the node enters the scene tree for the first time.
func _enter_tree():
	ovr = OpenVRInterface.get_interface()

func _load_controller_mesh(controller_name):
	# TODO: This can never actually return false, the fallback is useless.
	var model: ArrayMesh
	model = ovr.load_render_model(controller_name)
	if model:
		return model

	model = ovr.load_render_model("generic_controller")
	if model:
		return model

	return Mesh.new()

func _process(_delta):
	var controller: XRController3D
	var model_name = render_model_name

	if not render_model_name:
		# TODO: Actually do the autodetection, this doesn't work
		if not get_parent() is XRController3D:
			print("Can't detect render model, please specify one")
			return

		controller = get_parent()

		if controller and !controller.get_is_active():
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
		model_name = controller_name.substr(0, controller_name.length()-2)  # XXX: why?

	# attempt to load a mesh for this
	# TODO: can't tell if this actually worked, need a way to reset if the load function returns an error later on in the render thread
	if not controller_is_loaded:
		mesh = _load_controller_mesh(model_name)
		controller_is_loaded = true
