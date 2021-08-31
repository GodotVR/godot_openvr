extends Node3D

class_name Framecounter3D

func _ready():
	var material : ShaderMaterial = $Display.get_active_material(0)
	material.set_shader_param("viewport_texture", $Viewport.get_texture())

func _process(delta):
	$Viewport/Label.text = str(Engine.get_frames_per_second()) + " FPS"
