extends Node2D

# When we use our GLES3 renderer Godot expects that we do our color conversion to sRGB
# OpenVR wants a buffer without this conversion applied and leave the color in linear color space.
# As a result we've turned off our sRGB conversion on our viewport.
# To output with the correct colors to screen we output our VR preview using a TextureRect
# with a linear to sRGB conversion shader.

func _on_window_size_change():
	$TextureRect.rect_size = OS.window_size

# Called when the node enters the scene tree for the first time.
func _ready():
	$TextureRect.texture = $Viewport.get_texture()

	get_tree().get_root().connect("size_changed", _on_window_size_change)
	_on_window_size_change();

