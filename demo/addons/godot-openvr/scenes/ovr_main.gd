extends XROrigin3D

class_name OVRMain3D

# Add this script to your XROrigin3D node and it will initialise OpenVR for you automatically.

# Our plugin will now use the first actions.json found in the following locations
# 1) in the actions folder alongside the executable
# 2) in the actions folder within your project folder (i.e. "res://actions/actions.json")
# 3) in the actions folder within the plugin (i.e. "res://addons/godot-openvr/actions/actions.json")
# OpenVR can't read actions files within the exported datapack.

# The plugin always registers atleast one action set.
# If you have renamed this action set you can specify the name here
@export var default_action_set : String = "/actions/godot"

# If we render to a custom viewport give our node path here.
@export var viewport : NodePath

# Convenience setting for setting physics update rate to a multiple of our HMDs frame rate (set to 0 to ignore)
@export var physics_factor : float = 2

var xr_interface : XRInterfaceOpenVR = null
func get_xr_interface() -> XRInterfaceOpenVR:
	return xr_interface

func _ready():
	# Find the interface and initialise
	xr_interface = XRServer.find_interface("OpenVR")
	if xr_interface:
		# Configure our interface before initializing
		xr_interface.default_action_set = default_action_set

		if xr_interface.initialize():
			# We can't query our HMDs refresh rate just yet so we hardcode this to 90
			var refresh_rate = 90

			# check our viewport
			var vp : Viewport
			if viewport:
				vp = get_node(viewport)
				if vp:
					# We copy this, while the ARVRServer will resize the size of the viewport automatically
					# it can't feed it back into the node. 
					vp.size = xr_interface.get_render_targetsize()

			# No viewport? get our main viewport
			if !vp:
				vp = get_viewport()

			# switch to ARVR mode
			vp.use_xr = true

			# make sure vsync is disabled or we'll be limited to 60fps (or whatever the limit of your monitor is)
			DisplayServer.window_set_vsync_mode(DisplayServer.VSYNC_DISABLED)

			if physics_factor > 0:
				# Set our physics to a multiple of our refresh rate to get in sync with our rendering
				Engine.physics_ticks_per_second = refresh_rate * physics_factor

