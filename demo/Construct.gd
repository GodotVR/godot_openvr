extends Node2D

# When we use our GLES3 renderer Godot expects that we do our color conversion to sRGB
# OpenVR wants a buffer without this conversion applied and leave the color in linear color space.
# As a result we've turned off our sRGB conversion on our viewport.
# To output with the correct colors to screen we output our VR preview using a TextureRect
# with a linear to sRGB conversion shader. 

func on_window_size_change():
	$TextureRect.rect_size = OS.window_size

# Called when the node enters the scene tree for the first time.
func _ready():
	$TextureRect.texture = $Viewport.get_texture()
	
	get_tree().get_root().connect("size_changed", self, "on_window_size_change")
	on_window_size_change();


# Manually toggle pause
func _on_Pause_released():
	get_tree().paused = !get_tree().paused

# If the dashboard is open, hide the player to avoid depth confusion
# The dashboard renders its own models for the controllers
func _on_PauseOnDashboard_dashboard_opened():
	$Viewport/Main/Player.visible = false

# Restore our player when the dashboard closes
func _on_PauseOnDashboard_dashboard_closed():
	$Viewport/Main/Player.visible = true
