extends Node

@export var is_active: bool = true
@export var haptic_action: String = "/actions/godot/out/haptic":
	set(new_value):
		haptic_action = new_value
		_update_haptic()

@export_enum("any", "left", "right") var on_hand : int = 0:
	set(new_value):
		on_hand = new_value
		_update_on_hand()

var was_active = false
var was_colliding = false

func _update_haptic():
	if $Haptic:
		$Haptic.action = haptic_action

func _update_on_hand():
	if $Haptic:
		$Haptic.on_hand = on_hand

# Called when the node enters the scene tree for the first time.
func _ready():
	# When this is called with our default value our haptic node wasn't ready yet so we set it to itself
	_update_haptic()
	_update_on_hand()

func _process(delta):
	var parent = get_parent()
	if parent:
		if !is_active:
			was_active = false
		elif !was_active:
			# reset to the position of our parent
			$KinematicBody.global_transform.origin = parent.global_transform.origin
			was_active = true
			was_colliding = false
		else:
			# attempt to move our kinematic body to where our hand is
			var rel_vec = parent.global_transform.origin - $KinematicBody.global_transform.origin
			var collider = $KinematicBody.move_and_collide(rel_vec)
			
			if collider:
				if !was_colliding:
					was_colliding = true
					$Haptic.trigger_pulse()
			else:
				was_colliding = false
			
			# if we are obstructed we should check if our hand is now somewhere it would no longer collide with something and move our kinematic body there.
			

