extends "res://addons/vr-common/objects/Object_pickable.gd"

export (PackedScene) var casing = null
export var initial_bullet_velocity = 150.0
export var initial_casing_velocity = 5.0
export (PackedScene) var bullet = null

export var mag_size = 30

var ammo = 0
var firing = false
var controller = null

# note, we (ab)use our animation players is playing status as state

func set_ammo(new_ammo):
	ammo = clamp(new_ammo, 0, mag_size)
	
	# update viewport
	var si = $Pivot/AmmoCount.get_scene_instance()
	if si:
		si.ammo = ammo

func get_ammo():
	return ammo

func _fire():
	if !$AnimationPlayer.is_playing():
		if ammo > 0:
			# decrease ammo
			set_ammo(get_ammo() - 1)
		
			# spawn bullet
			var new_bullet = bullet.instance()
			new_bullet.transform = $Pivot/BulletSpawnPoint.global_transform
			new_bullet.set_initial_velocity(new_bullet.transform.basis.z * initial_bullet_velocity)
			get_node("/root/Main/Spawns").add_child(new_bullet)
			
			# spawn casing
			var new_casing = casing.instance()
			new_casing.transform = $Pivot/CasingSpawnPoint.global_transform
			new_casing.linear_velocity = new_casing.transform.basis.x * initial_casing_velocity
			get_node("/root/Main/Spawns").add_child(new_casing)
			
			# simple recoil
			$Pivot.rotation.x = clamp($Pivot.rotation.x + rand_range(0.0, 0.2), 0.0, 0.2)
			$Pivot.rotation.y = clamp($Pivot.rotation.y + rand_range(-0.1, 0.1), -0.1, 0.1)
			
			# play our shoot animation
			$AnimationPlayer.play("Shoot")
		else:
			# empty!
			$AnimationPlayer.play("Empty")

func _on_button_pressed(button):
	if button == JOY_VR_TRIGGER:
		firing = true
		_fire()

func _on_button_release(button):
	if button == JOY_VR_TRIGGER:
		firing = false

func pick_up(by, with_controller):
	# call our original code
	.pick_up(by,with_controller)
	
	# connect to signals ourselves
	controller = with_controller
	controller.connect("button_pressed", self, "_on_button_pressed")
	controller.connect("button_release", self, "_on_button_release")

func let_go(impulse = Vector3(0.0, 0.0, 0.0)):
	# make sure we're not firing and reset our rotation
	firing = false
	$Pivot.rotation.x = 0.0
	
	# disconnect
	controller.disconnect("button_pressed", self, "_on_button_pressed")
	controller.disconnect("button_release", self, "_on_button_release")
	controller = null
	
	# call our original code
	.let_go(impulse)

func _ready():
	set_ammo(mag_size)

func _process(delta):
	if $Pivot.rotation.x != 0.0:
		if $Pivot.rotation.x > 0:
			$Pivot.rotation.x = clamp($Pivot.rotation.x - delta, 0.0, $Pivot.rotation.x)
		else:
			$Pivot.rotation.x = clamp($Pivot.rotation.x + delta, $Pivot.rotation.x, 0.0)

	if $Pivot.rotation.y != 0.0:
		if $Pivot.rotation.y > 0:
			$Pivot.rotation.y = clamp($Pivot.rotation.y - delta, 0.0, $Pivot.rotation.y)
		else:
			$Pivot.rotation.y = clamp($Pivot.rotation.y + delta, $Pivot.rotation.y, 0.0)

func _on_Reload_body_entered(body):
	if !$AnimationPlayer.is_playing():
		# our ammo is loaded, so we can destroy our magazine
		body.drop_and_free()
		
		# and reload
		set_ammo(mag_size)
		
		# and finally play reload animation
		$AnimationPlayer.play("Load")

func _on_AnimationPlayer_animation_finished(_anim_name):
	# we can fire again
	if firing:
		# fire another bullet
		_fire()
