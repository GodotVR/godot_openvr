extends RigidBody

var last_velocity = Vector3(0.0, 0.0, 0.0)
var last_position = Vector3(0.0, 0.0, 0.0)
var first = true

var spring_factor = 5.0

func _physics_process(delta):
	# implement a poor mans spring effect
	if first:
		first = false
	else:
		var projected_position = last_position + last_velocity * delta
		var delta_position =  global_transform.origin - projected_position
		
		projected_position += delta_position * delta * spring_factor
		$Offset.global_transform.origin = projected_position
		
		last_velocity = ($Offset.global_transform.origin - last_position) / delta
	
	last_position = $Offset.global_transform.origin


func _on_Lifetime_timeout():
	queue_free()


func _on_Area_body_entered(body):
	print("Got hit by " + body.name)
