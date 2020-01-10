extends Node2D

export var ammo = 30 setget set_ammo

var is_ready = true

func set_ammo(new_count):
	ammo = new_count
	if is_ready:
		$Label.text = str(ammo)

# Called when the node enters the scene tree for the first time.
func _ready():
	is_ready = true
	set_ammo(ammo)
