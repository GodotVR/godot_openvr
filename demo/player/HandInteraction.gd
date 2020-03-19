extends Node

export var is_active = true
export var haptic_action = "/actions/godot/out/haptic" setget set_haptic
export (int, "any", "left", "right") var on_hand = 0 setget set_on_hand

var was_active = false
var was_colliding = false

func set_haptic(new_action):
	haptic_action = new_action
	if $Haptic:
		$Haptic.action = haptic_action

func set_on_hand(new_on_hand):
	on_hand = new_on_hand
	if $Haptic:
		$Haptic.on_hand = on_hand

# Called when the node enters the scene tree for the first time.
func _ready():
	# When this is called with our default value our haptic node wasn't ready yet so we set it to itself
	set_haptic(haptic_action)
	set_on_hand(on_hand)

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
			

