extends "res://addons/godot-xr-tools/objects/Object_pickable.gd"

var color_idx = 0
var colors = [ Color(1.0, 0.0, 0.0, 1.0), Color(0.0, 1.0, 0.0, 1.0), Color(0.0, 0.0, 1.0, 1.0) ]
var material : StandardMaterial3D = null

var impulse_factor = 0.1

func _ready():
	material = $MeshInstance.get_surface_material(0)
	material.albedo_color = colors[color_idx]

func _on_Action_pressed():
	color_idx = color_idx + 1
	if (color_idx == 3):
		color_idx = 0
	
	material.albedo_color = colors[color_idx]

func _physics_process(delta):
	var analog = $Action.get_analog()
	if (analog.x != 0.0):
		apply_central_impulse(global_transform.basis.x * analog.x * impulse_factor)
	if (analog.y != 0.0):
		apply_central_impulse(global_transform.basis.z * analog.y * impulse_factor)
