extends "res://addons/godot-openvr/scenes/ovr_controller.gd"

# Called when the node enters the scene tree for the first time.
func _ready():
	$Viewport2Din3D.get_scene_instance().set_controller(self)
