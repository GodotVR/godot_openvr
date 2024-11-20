extends "res://addons/godot-openvr/scenes/ovr_hud_anchor.gd"

func _process(_delta):
	var fps = Performance.get_monitor(Performance.TIME_FPS)
	$FPS.text = "FPS: " + str(fps)
