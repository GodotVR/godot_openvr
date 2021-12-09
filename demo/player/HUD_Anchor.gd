extends "res://addons/godot-openvr/scenes/ovr_hud_anchor.gd"

func _ready():
	var material : StandardMaterial3D = $FPSQuad.get_surface_override_material(0)
	if material:
		material.albedo_texture = $SubViewport.get_texture()

func _process(delta):
	var fps = Performance.get_monitor(Performance.TIME_FPS)
	$SubViewport/FPS.text = "FPS: " + str(fps)
