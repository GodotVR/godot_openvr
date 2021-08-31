extends XRController3D

class_name OVRController3D

signal controller_activated(controller)

@export var show_controller_mesh = true:
	set(p_show):
		show_controller_mesh = p_show
		if $OVRRenderModel:
			$OVRRenderModel.visible = p_show
	get:
		return show_controller_mesh

func _ready():
	# set our starting vaule
	$OVRRenderModel.visible = show_controller_mesh
	
	# hide to begin with
	visible = false

func _process(delta):
	if !get_is_active():
		visible = false
		return
	
	if visible:
		return
	
	# make it visible
	visible = true
	emit_signal("controller_activated", self)
