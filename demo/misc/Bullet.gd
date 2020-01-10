extends KinematicBody

export (Material) var trail_material = null
export var max_segments = 15
export var width = 0.05

var velocity = Vector3(0.0, 0.0, 0.0)
var gravity = 9.8

var trail = Array()
var mesh: ArrayMesh
var passed = 100.0
var max_passed = 0.1

func set_initial_velocity(new_velocity):
	velocity = new_velocity

# Called when the node enters the scene tree for the first time.
func _ready():
	# create a unique mesh
	mesh = ArrayMesh.new()
	$Detach/Trail.mesh = mesh

func _physics_process(delta):
	# While our bullet is visible, animate it...
	if $BulletMesh.visible:
		
		# new_bullet.transform = t.looking_at(t.origin + t.basis.z, t.basis.y)
		
		# apply gravity
		velocity.y = velocity.y - delta * gravity
		
		var collision = move_and_collide(delta * velocity)
		if collision:
			print("We hit " + collision.collider.name)
			
			# check if what we hit has a hit action
			if collision.collider.has_method("hit"):
				collision.collider.hit()
			
			finish()

func finish():
	# hide our bullet so we can finish our trial.
	$BulletMesh.visible = false
	velocity = Vector3(0.0, 0.0, 0.0)

func _process(delta):
	passed = passed + delta
	if passed < max_passed:
		return
	
	# reset
	passed = 0.0
	
	# add our current location
	trail.push_back(global_transform)
	
	# check if we need to update our
	if trail.size() > 2:
		if trail.size() > max_segments:
			trail.pop_front()
		
		var segments = trail.size()
		
		var mesh_array = Array()
		mesh_array.resize(ArrayMesh.ARRAY_MAX)
		var v = PoolVector3Array()
		v.resize(segments * 4)
		var uv = PoolVector2Array()
		uv.resize(segments * 4)
		var idx = PoolIntArray()
		idx.resize(segments * 12)
		
		# this is not super fast but it will do...
		var i = 0;
		var f = 0;
		for segment in trail:
			v[i] = segment.origin - segment.basis.x * width * 0.5
			uv[i] = Vector2(0.0, i / segments)
			
			v[i + 1] = segment.origin + segment.basis.x * width * 0.5
			uv[i + 1] = Vector2(1.0, i / segments)
			
			v[i + 2] = segment.origin - segment.basis.y * width * 0.5
			uv[i + 2] = Vector2(0.0, i / segments)
			
			v[i + 3] = segment.origin + segment.basis.y * width * 0.5
			uv[i + 3] = Vector2(1.0, i / segments)
			
			idx[f] = i - 4 
			idx[f + 1] = i
			idx[f + 2] = i + 1
			
			idx[f + 3] = i - 4 
			idx[f + 4] = i + 1
			idx[f + 5] = i - 3
			
			idx[f + 6] = i - 2
			idx[f + 7] = i + 2
			idx[f + 8] = i + 3
			
			idx[f + 9] = i - 2 
			idx[f + 10] = i + 3
			idx[f + 11] = i - 1
			
			i = i + 4
			f = f + 12
		
		mesh_array[ArrayMesh.ARRAY_VERTEX] = v
		mesh_array[ArrayMesh.ARRAY_TEX_UV] = uv
		mesh_array[ArrayMesh.ARRAY_INDEX] = idx
		
		if mesh.get_surface_count() > 0:
			mesh.surface_remove(0)
		mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES,mesh_array)
		mesh.surface_set_material(0, trail_material)

func _on_Timer_timeout():
	get_node("/root/Main/Spawns").remove_child(self)
	queue_free()

#func _on_Bullet_body_entered(body):
#	# we hit something!
#	print("Hit " + body.name)
