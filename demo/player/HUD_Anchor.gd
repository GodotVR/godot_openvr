extends "res://addons/godot-openvr/scenes/ovr_hud_anchor.gd"


func _ready() -> void:
	# A sampling of VREvents which are exposed as signals:
	OpenVRInterface.DashboardActivated.connect(_handle_vrevent.bind("Dashboard open"))
	OpenVRInterface.DashboardDeactivated.connect(_handle_vrevent.bind("Dashboard closed"))
	OpenVRInterface.TrackedDeviceActivated.connect(_handle_vrevent.bind("Found device"))
	OpenVRInterface.TrackedDeviceDeactivated.connect(_handle_vrevent.bind("Lost device"))

func _process(_delta):
	var fps = Performance.get_monitor(Performance.TIME_FPS)
	$FPS.text = "FPS: " + str(fps)


func _handle_vrevent(_age, tracker, _data, message=""):
	var lines: Array = $Events.text.split("\n")
	lines.push_front(message + (": " + tracker.name if tracker else ""))
	$Events.text = "\n".join(lines.slice(0, 2))
