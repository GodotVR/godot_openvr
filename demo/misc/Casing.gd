extends RigidBody

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func _on_Life_timeout():
	get_node("/root/Main/Spawns").remove_child(self)
	queue_free()
