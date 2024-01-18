@tool
extends EditorExportPlugin

func _export_begin(features: PackedStringArray, is_debug: bool, path: String, flags: int ):
	var dir = DirAccess.open('res://')
	
	# we just want the path
	var export_to = path.get_base_dir() + "/actions/"
	
	# now determine which action files to export
	var export_from = ""
	if dir.file_exists("actions/actions.json"):
		export_from = "actions/"
	elif dir.file_exists("addons/godot-openvr/actions/actions.json"):
		export_from = "addons/godot-openvr/actions/"
	else:
		print("WARNING: Couldn't locate actions files to export")
		return
	
	# print("Exporting " + export_from +" to " + export_to)
	if !dir.dir_exists(export_to):
		dir.make_dir(export_to)
	
	dir = dir.open(export_from)
	if dir:
		dir.include_hidden = false
		dir.include_navigational = false
		dir.list_dir_begin()
		
		var filename = dir.get_next()
		while filename != "":
			# print("Copy " + filename)
			dir.copy(export_from + filename, export_to + filename)
			
			filename = dir.get_next()
		
		dir.list_dir_end()

#func _export_end():
#	print("Export ended")

#func _export_file(path: String, type: String, features: PoolStringArray):
#	print("Export " + path)
