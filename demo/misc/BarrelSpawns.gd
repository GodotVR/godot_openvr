extends Spatial

export (PackedScene) var Spawn = null

func start_timer():
	$SpawnTimer.wait_time = rand_range(1.5,4.0)
	$SpawnTimer.start()

# Called when the node enters the scene tree for the first time.
func _ready():
	start_timer()

func _on_Timer_timeout():
	# spawn a new barrel.. 
	if Spawn:
		var new_spawn = Spawn.instance()
		add_child(new_spawn)
		
		# and restart
		start_timer()
