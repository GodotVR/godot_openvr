extends MeshInstance3D

class_name OVRRenderModel3D

var ovr: XRInterfaceOpenVR

var ws = 0

## Keep track of which render model is already loaded so we don't flicker by
## replacing it with the same one if tracker signals are received but the
## controller didn't actually change.
var _loaded_render_model_name: String = ""

var _xr_tracker:
	get:
		var parent = get_parent()
		if parent is XRController3D:
			return parent.tracker

		return ""


## Override the render model. If unset and the parent of this node is an XRController3D,
## an attempt will be made to locate the correct model automatically.
@export var render_model_name: String:
	get:
		if render_model_name:
			return render_model_name
		elif _xr_tracker:
			# The openvr interface sets the tracker description to the render model id.
			var tracker = XRServer.get_tracker(_xr_tracker)
			if tracker:
				return tracker.description

		return ""

	set(name):
		render_model_name = name
		update_rendermodel()


func _enter_tree():
	ovr = OpenVRInterface.get_interface()

	XRServer.tracker_added.connect(_on_tracker_changed, CONNECT_DEFERRED)
	XRServer.tracker_updated.connect(_on_tracker_changed, CONNECT_DEFERRED)
	XRServer.tracker_removed.connect(_on_tracker_changed, CONNECT_DEFERRED)


func _ready():
	update_rendermodel()


func _physics_process(_delta):
	var new_ws = XRServer.world_scale
	if (ws != new_ws):
		ws = new_ws
		scale = Vector3(ws, ws, ws)


func _on_tracker_changed(tracker_name, _type):
	print_debug("tracker changed: " + tracker_name)
	if tracker_name == _xr_tracker:
		update_rendermodel()


func update_rendermodel():
	if render_model_name and render_model_name != _loaded_render_model_name:
		mesh = ovr.load_render_model(render_model_name)
		_loaded_render_model_name = render_model_name
	else:
		mesh = null
		_loaded_render_model_name = ""
