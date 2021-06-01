extends MeshInstance3D

@export var height: float = 2.0

var ws: float = 0.0
var reference: Transform3D = Transform3D()

func _ready():
	# todo subscribe to our play area changed signal
	pass

func _process(delta):
	# if either of these change, our play area needs to be adjusted.
	var new_ws = XRServer.world_scale
	var new_reference = XRServer.get_reference_frame()
	
	if ws!=new_ws or reference!=new_reference:
		var openvr_config = get_parent().get_openvr_config()
		if !openvr_config:
			# can't update this yet
			return
		
		if !openvr_config.play_area_available():
			# can't update this yet
			return
		
		# get our play area, ws and our reference frame have already been applied
		var play_area = openvr_config.get_play_area()
		var h = Vector3(0.0, height, 0.0)
		
		var st = SurfaceTool.new()
		st.begin(Mesh.PRIMITIVE_TRIANGLES)
		
		# floor
		st.add_uv(Vector2(0,0))
		st.add_vertex(play_area[0])
		st.add_uv(Vector2(1,1))
		st.add_vertex(play_area[2])
		st.add_uv(Vector2(1,0))
		st.add_vertex(play_area[1])
		st.add_uv(Vector2(0,0))
		st.add_vertex(play_area[0])
		st.add_uv(Vector2(0,1))
		st.add_vertex(play_area[3])
		st.add_uv(Vector2(1,1))
		st.add_vertex(play_area[2])
		
		# ceiling
		st.add_uv(Vector2(0,0))
		st.add_vertex(play_area[0] + h)
		st.add_uv(Vector2(1,0))
		st.add_vertex(play_area[1] + h)
		st.add_uv(Vector2(1,1))
		st.add_vertex(play_area[2] + h)
		st.add_uv(Vector2(0,0))
		st.add_vertex(play_area[0] + h)
		st.add_uv(Vector2(1,1))
		st.add_vertex(play_area[2] + h)
		st.add_uv(Vector2(0,1))
		st.add_vertex(play_area[3] + h)
		
		# side A
		st.add_uv(Vector2(0,0))
		st.add_vertex(play_area[0])
		st.add_uv(Vector2(1,1))
		st.add_vertex(play_area[1] + h)
		st.add_uv(Vector2(1,0))
		st.add_vertex(play_area[0] + h)
		st.add_uv(Vector2(0,0))
		st.add_vertex(play_area[0])
		st.add_uv(Vector2(0,1))
		st.add_vertex(play_area[1])
		st.add_uv(Vector2(1,1))
		st.add_vertex(play_area[1] + h)
		
		# side B
		st.add_uv(Vector2(0,0))
		st.add_vertex(play_area[3])
		st.add_uv(Vector2(1,1))
		st.add_vertex(play_area[0] + h)
		st.add_uv(Vector2(1,0))
		st.add_vertex(play_area[3] + h)
		st.add_uv(Vector2(0,0))
		st.add_vertex(play_area[3])
		st.add_uv(Vector2(0,1))
		st.add_vertex(play_area[0])
		st.add_uv(Vector2(1,1))
		st.add_vertex(play_area[0] + h)
		
		# side C
		st.add_uv(Vector2(0,0))
		st.add_vertex(play_area[1])
		st.add_uv(Vector2(1,1))
		st.add_vertex(play_area[2] + h)
		st.add_uv(Vector2(1,0))
		st.add_vertex(play_area[1] + h)
		st.add_uv(Vector2(0,0))
		st.add_vertex(play_area[1])
		st.add_uv(Vector2(0,1))
		st.add_vertex(play_area[2])
		st.add_uv(Vector2(1,1))
		st.add_vertex(play_area[2] + h)
		
		# side D
		st.add_uv(Vector2(0,0))
		st.add_vertex(play_area[2])
		st.add_uv(Vector2(1,1))
		st.add_vertex(play_area[3] + h)
		st.add_uv(Vector2(1,0))
		st.add_vertex(play_area[2] + h)
		st.add_uv(Vector2(0,0))
		st.add_vertex(play_area[2])
		st.add_uv(Vector2(0,1))
		st.add_vertex(play_area[3])
		st.add_uv(Vector2(1,1))
		st.add_vertex(play_area[3] + h)
		
		st.generate_normals()
		st.generate_tangents()
		
		# we've updated it
		mesh = st.commit()
		ws = new_ws
		reference = new_reference
