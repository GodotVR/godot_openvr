extends XRController3D

class_name OVRController3D


@export var show_controller_mesh = true:
	set(p_show):
		show_controller_mesh = p_show
		$OVRRenderModel.visible = p_show
	get:
		return show_controller_mesh


func _ready():
	$OVRRenderModel.visible = show_controller_mesh
