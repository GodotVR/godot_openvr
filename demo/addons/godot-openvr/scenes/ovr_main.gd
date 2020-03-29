extends ARVROrigin

# Add this script to your ARVROrigin node and it will initialise OpenVR for you automatically.

# Our default action bindings can be found at: res://addons/godot-openvr/actions/actions.json
# You can specify your own custom action file here.
# Note that you can take out all default actions if you wish to use your own but
# features such as ARVRController.get_joystick_axis and ARVRController.is_button_pressed won't work.
export (String) var action_json_path = "res://addons/godot-openvr/actions/actions.json"

# The plugin always registers atleast one action set.
# If you have renamed this action set you can specify the name here
export (String) var default_action_set = "/actions/godot"

# If we render to a custom viewport give our node path here.
export (NodePath) var viewport = null

var arvr_interface : ARVRInterface = null
var openvr_config = null

func get_openvr_config():
	return openvr_config

func _ready():
	# Load our config before we initialise
	openvr_config = preload("res://addons/godot-openvr/OpenVRConfig.gdns");
	if openvr_config:
		print("Setup configuration")
		openvr_config = openvr_config.new()
		
		openvr_config.action_json_path = action_json_path
		openvr_config.default_action_set = default_action_set

	# Find the interface and initialise
	arvr_interface = ARVRServer.find_interface("OpenVR")
	if arvr_interface and arvr_interface.initialize():
		var vp : Viewport = null
		if viewport:
			vp = get_node(viewport)
			if vp:
				# We copy this, while the ARVRServer will resize the size of the viewport automatically
				# it can't feed it back into the node. 
				vp.size = arvr_interface.get_render_targetsize()
				
		
		# No viewport? get our main viewport
		if !vp:
			vp = get_viewport()
		
		# switch to ARVR mode
		vp.arvr = true
		
		# keep linear color space, not needed and thus ignored with the GLES2 renderer
		vp.keep_3d_linear = true
		
		# make sure vsync is disabled or we'll be limited to 60fps
		OS.vsync_enabled = false
		
		# up our physics to 90fps to get in sync with our rendering
		Engine.iterations_per_second = 90

