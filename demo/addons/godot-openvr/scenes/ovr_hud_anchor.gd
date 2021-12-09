extends Node3D

@export var camera : NodePath

func _physics_process(delta):
	# we keep this in line with our camera but we remove the tilt
	if camera:
		var camera_node = get_node(camera)
		if camera_node:
			var new_transform : Transform3D = camera_node.transform

			var new_basis = Basis()
			new_basis.z = Vector3(new_transform.basis.z.x, 0.0, new_transform.basis.z.z).normalized()
			if new_basis.z.length() > 0.5:
				new_basis.y - Vector3(0.0, 1.0, 0.0)
				new_basis.x = new_basis.y.cross(new_basis.z)
				new_transform.basis = new_basis

				transform = new_transform
