////////////////////////////////////////////////////////////////////////////////////////////////
// Helper calls and singleton container for accessing openvr

#include "openvr_data.h"
#include "Time.hpp"
#include "Utilities.hpp"
#include "XRServer.hpp"

using namespace godot;

openvr_data *openvr_data::singleton = NULL;

openvr_data::openvr_data() {
	// get some default values
	use_count = 1;
	hmd = NULL;
	render_models = NULL;

	application_type = OpenVRApplicationType::SCENE;
	tracking_universe = OpenVRTrackingUniverse::STANDING;

	chaperone = NULL;
	play_area_is_dirty = true;
	for (int i = 0; i < 4; i++) {
		play_area[i].x = 0.0f;
		play_area[i].y = 0.0f;
		play_area[i].z = 0.0f;
	}

	int default_action_set = register_action_set(String("/actions/godot"));
	action_sets[default_action_set].is_active = true;
	active_action_set_count = 1;
}

openvr_data::~openvr_data() {
	cleanup();

	if (singleton == this) {
		singleton = NULL;
	}
}

void openvr_data::cleanup() {
	if (hmd != NULL) {
		// reset our action handles
		for (int i = 0; i < DAH_IN_MAX; i++) {
			input_action_handles[i] = vr::k_ulInvalidActionHandle;
		}
		for (int i = 0; i < DAH_OUT_MAX; i++) {
			output_action_handles[i] = vr::k_ulInvalidActionHandle;
		}

		// detach all our devices
		for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
			detach_device(i);
		}

		// forget our custom actions

		// forget our action sets
		for (std::vector<action_set>::iterator it = action_sets.begin(); it != action_sets.end(); ++it) {
			it->handle = vr::k_ulInvalidActionSetHandle;
		}

		hmd = NULL;
		render_models = NULL;
		chaperone = NULL;

		vr::VR_Shutdown();
	}
}

openvr_data *openvr_data::retain_singleton() {
	if (singleton == NULL) {
		singleton = new openvr_data();
	} else {
		singleton->use_count++;

		String msg("Usage count increased to {0}");
		Array arr;
		arr.push_back(Variant(singleton->use_count));

		Utilities::print(msg.format(arr));
	}
	return singleton;
}

void openvr_data::release() {
	if (singleton != this) {
		// this should never happen!
		Utilities::print("openvr object does not match singleton!");
	} else if (singleton->use_count > 1) {
		singleton->use_count--;

		String msg("Usage count decreased to {0}");
		Array arr;
		arr.push_back(Variant(singleton->use_count));

		Utilities::print(msg.format(arr));
	} else {
		delete this;
	}
}

bool openvr_data::is_initialised() {
	return hmd != NULL;
}

bool openvr_data::initialise() {
	if (hmd != NULL) {
		// already initialised, no need to do again
		return true;
	}

	// init openvr
	Utilities::print("OpenVR: initialising OpenVR context\n");

	bool success = true;
	vr::EVRInitError error = vr::VRInitError_None;

	if (!vr::VR_IsRuntimeInstalled()) {
		Utilities::print("SteamVR has not been installed.");
		success = false;
	}

	if (success) {
		// Loading the SteamVR Runtime
		if (application_type == OpenVRApplicationType::OVERLAY) {
			hmd = vr::VR_Init(&error, vr::VRApplication_Overlay);
			Utilities::print("Application in overlay mode.");
		} else {
			hmd = vr::VR_Init(&error, vr::VRApplication_Scene);
			Utilities::print("Application in scene (normal) mode.");
		}

		if (error != vr::VRInitError_None) {
			success = false;

			Utilities::print(String("Unable to init VR runtime: {0}").format(Array::make(String(vr::VR_GetVRInitErrorAsEnglishDescription(error)))));
		} else {
			Utilities::print("Main OpenVR interface has been initialized");
		}
	}

	if (success) {
		// render models give us access to mesh representations of the various
		// controllers
		render_models = (vr::IVRRenderModels *)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &error);
		if (!render_models) {
			success = false;

			Utilities::print(godot::String("Unable to get render model interface: ") + godot::String(vr::VR_GetVRInitErrorAsEnglishDescription(error)));
		} else {
			Utilities::print("Main render models interface has been initialized.");
		}
	}

	if (success) {
		if (!vr::VRCompositor()) {
			success = false;

			Utilities::print("Compositor initialization failed. See log file for details.");
		}
	}

	if (success) {
		if (application_type == OpenVRApplicationType::OVERLAY) {
			if (!vr::VROverlay()) {
				success = false;

				Utilities::print("Overlay system initialization failed. See log file for details.");
			}
		}
	}

	if (success) {
		chaperone = vr::VRChaperone();
		if (chaperone == NULL) {
			success = false;

			Utilities::print("Chaperone initialization failed. See log file for details.");
		} else {
			update_play_area();
		}
	}

	if (success) {
		OS *os = OS::get_singleton();
		Directory *directory = Directory::_new();
		String exec_path = os->get_executable_path().get_base_dir().replace("\\", "/");
		String manifest_path;

		Utilities::print(String("Exec path: {0}").format(Array::make(String(exec_path))));

		// check 3 locations in order
		// 1) check if we have an action folder alongside our executable (runtime deployed actions)
		String path = String("{0}{1}").format(Array::make(String(exec_path), String("/actions/actions.json")));
		if (directory->file_exists(path)) {
			manifest_path = path;
		} else {
			String project_path = ProjectSettings::get_singleton()->globalize_path("res://");
			if (project_path.length() != 0) {
				Utilities::print(String("Project path: {0}").format(Array::make(project_path)));

				// 2) else check if we have an action folder in our project folder (custom user actions in development)
				path = String("{0}{1}").format(Array::make(project_path, String("actions/actions.json")));
				if (directory->file_exists(path)) {
					manifest_path = path;
				} else {
					// 3) else check if we have an action folder in our plugin (if no user overrule)
					path = String("{0}{1}").format(Array::make(project_path, String("addons/godot-openvr/actions/actions.json")));
					if (directory->file_exists(path)) {
						manifest_path = path;
					}
				}
			}
		}

		if (manifest_path.length() != 0) {
			vr::EVRInputError err = vr::VRInput()->SetActionManifestPath((const char *)manifest_path);
			if (err == vr::VRInputError_None) {
				Utilities::print(String("Loaded action json from: {0}").format(Array::make(manifest_path)));
			} else {
				success = false;
				Utilities::print(String("Failed to load action json from: {0}").format(Array::make(manifest_path)));
			}
		} else {
			success = false;
			Utilities::print(godot::String("Failed to find action file"));
		}

		directory->free();
	}

	if (success) {
		// TODO: Contemplate whether we should parse the JSON ourselves so we know what actions and action sets are available...
		// we can then get action handles for all of them automatically.

		bind_default_action_handles();

		for (std::vector<action_set>::iterator it = action_sets.begin(); it != action_sets.end(); ++it) {
			vr::EVRInputError err = vr::VRInput()->GetActionSetHandle((const char *)it->name, &it->handle);
			if (err != vr::VRInputError_None) {
				Utilities::print(String("Failed to obtain action set handle for {0}").format(Array::make(String(it->name))));
			}
		}

		for (std::vector<custom_action>::iterator it = custom_actions.begin(); it != custom_actions.end(); ++it) {
			vr::EVRInputError err = vr::VRInput()->GetActionHandle((const char *)it->name, &it->handle);
			if (err == vr::VRInputError_None) {
				Utilities::print(String("Bound action {0} to {1}").format(Array::make(String(it->name), Variant((int64_t)it->handle))));
			} else {
				Utilities::print(String("Failed to obtain action handle for {0}").format(Array::make(String(it->name))));
			}
		}
	}

	if (success) {
		/* reset some stuff */
		for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
			tracked_devices[i].tracker_id = 0;
			tracked_devices[i].last_rumble_update = 0;
		}

		device_hands_are_available = false;
		left_hand_device = vr::k_unTrackedDeviceIndexInvalid;
		right_hand_device = vr::k_unTrackedDeviceIndexInvalid;

		// find any already attached devices
		for (uint32_t i = vr::k_unTrackedDeviceIndex_Hmd; i < vr::k_unMaxTrackedDeviceCount; i++) {
			if (is_tracked_device_connected(i)) {
				attach_device(i);
			}
		}
	}

	if (!success) {
		cleanup();
	}

	return success;
}

void openvr_data::update_play_area() {
	if (play_area_is_dirty && chaperone != NULL) {
		vr::HmdQuad_t new_rect;
		if (chaperone->GetPlayAreaRect(&new_rect)) {
			for (int i = 0; i < 4; i++) {
				play_area[i].x = new_rect.vCorners[i].v[0];
				play_area[i].y = new_rect.vCorners[i].v[1];
				play_area[i].z = new_rect.vCorners[i].v[2];
			}

			play_area_is_dirty = false;
		}
	}
}

void openvr_data::process() {
	// we need timing info for one or two things..
	uint64_t msec = godot::Time::get_singleton()->get_ticks_msec();

	// we scale all our positions by our world scale
	godot_float world_scale = godot::XRServer::get_singleton()->get_world_scale();

	// check our model loading in reverse
	for (int i = (int)load_models.size() - 1; i >= 0; i--) {
		if (_load_render_model(&load_models[i])) {
			load_models.erase(load_models.begin() + i);
		}
	}

	// check our texture loading in reverse
	for (int i = (int)load_textures.size() - 1; i >= 0; i--) {
		if (_load_texture(&load_textures[i])) {
			load_textures.erase(load_textures.begin() + i);
		}
	}

	vr::VREvent_t event;
	while (hmd->PollNextEvent(&event, sizeof(event))) {
		switch (event.eventType) {
			case vr::VREvent_TrackedDeviceActivated: {
				attach_device(event.trackedDeviceIndex);
			}; break;
			case vr::VREvent_TrackedDeviceDeactivated: {
				detach_device(event.trackedDeviceIndex);
			}; break;
			case vr::VREvent_ChaperoneDataHasChanged: {
				play_area_is_dirty = true;
			}; break;
			default: {
				// ignored for now...
			}; break;
		}
	}

	// update our play area data ?
	update_play_area();

	// Update our active action set
	if (active_action_set_count > 0) {
		int current_index = 0;

		// If the active action set count has changed, resize the array
		if (active_action_sets.size() != active_action_set_count) {
			active_action_sets.resize(active_action_set_count);
		}

		// Loop through all the action sets and add the ones requring update to the array
		for (int i = 0; i < action_sets.size(); i++) {
			if (action_sets[i].is_active) {
				vr::VRActiveActionSet_t actionSet = { 0 };
				actionSet.ulActionSet = action_sets[i].handle;

				active_action_sets[current_index] = actionSet;
				current_index++;
			}
		}

		// Pass the array to OpenVR
		if (current_index == active_action_set_count) {
			vr::VRInput()->UpdateActionState(active_action_sets.data(), sizeof(vr::VRActiveActionSet_t), active_action_set_count);
		}
	}

	// update our poses structure, this tracks our controllers
	vr::TrackedDevicePose_t tracked_device_pose[vr::k_unMaxTrackedDeviceCount];

	if (get_application_type() == openvr_data::OpenVRApplicationType::OVERLAY) {
		openvr_data::OpenVRTrackingUniverse tracking_universe = get_tracking_universe();
		if (tracking_universe == openvr_data::OpenVRTrackingUniverse::SEATED) {
			vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseSeated, 0.0, tracked_device_pose, vr::k_unMaxTrackedDeviceCount);
		} else if (tracking_universe == openvr_data::OpenVRTrackingUniverse::STANDING) {
			vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0.0, tracked_device_pose, vr::k_unMaxTrackedDeviceCount);
		} else {
			vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseRawAndUncalibrated, 0.0, tracked_device_pose, vr::k_unMaxTrackedDeviceCount);
		}
	} else {
		vr::VRCompositor()->WaitGetPoses(tracked_device_pose, vr::k_unMaxTrackedDeviceCount, NULL, 0);
	}

	// update trackers and joysticks
	for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
		// update tracker
		if (i == 0) {
			if (tracked_device_pose[i].bPoseIsValid) {
				// store our HMD transform
				hmd_transform = transform_from_matrix(&tracked_device_pose[i].mDeviceToAbsoluteTracking, world_scale);
			}
		} else if (tracked_devices[i].tracker_id != 0) {
			// We'll keep using our main transform we got from WaitGetPoses
			// To obtain specific poses use OpenVRPose
			if (tracked_device_pose[i].bPoseIsValid) {
				// update our location and orientation
				godot_transform3d transform;
				transform_from_matrix(&transform, &tracked_device_pose[i].mDeviceToAbsoluteTracking, world_scale);
				godot::xr_api->godot_xr_set_controller_transform(tracked_devices[i].tracker_id, &transform, true, true);
			}

			// for our fixed actions we'll hardcode checking our state
			process_device_actions(&tracked_devices[i], msec);
		}
	}
}

////////////////////////////////////////////////////////////////
// properties

int openvr_data::get_overlay_count() {
	return (int)overlays.size();
}

openvr_data::overlay openvr_data::get_overlay(int p_overlay_id) {
	return overlays[p_overlay_id];
}

int openvr_data::add_overlay(vr::VROverlayHandle_t p_new_value, godot::RID p_viewport_rid) {
	overlay new_entry;
	new_entry.handle = p_new_value;
	new_entry.viewport_rid = p_viewport_rid;

	overlays.push_back(new_entry);
	return (int)overlays.size() - 1;
}

void openvr_data::remove_overlay(int p_overlay_id) {
	overlays.erase(overlays.begin() + p_overlay_id);
}

openvr_data::OpenVRApplicationType openvr_data::get_application_type() {
	return application_type;
}

void openvr_data::set_application_type(openvr_data::OpenVRApplicationType p_new_value) {
	application_type = p_new_value;
}

openvr_data::OpenVRTrackingUniverse openvr_data::get_tracking_universe() {
	return tracking_universe;
}

void openvr_data::set_tracking_universe(openvr_data::OpenVRTrackingUniverse p_new_value) {
	tracking_universe = p_new_value;
}

bool openvr_data::play_area_available() const {
	return !play_area_is_dirty;
}

const godot::Vector3 *openvr_data::get_play_area() const {
	return play_area;
}

////////////////////////////////////////////////////////////////
// interact with openvr

void openvr_data::get_recommended_rendertarget_size(uint32_t *p_width, uint32_t *p_height) {
	if (hmd != NULL) {
		hmd->GetRecommendedRenderTargetSize(p_width, p_height);
	} else {
		// not sure why we're asking but give it a size...
		*p_width = 600;
		*p_height = 600;
	}
}

// todo - change this to String or keep char *?
char *openvr_data::get_device_name(vr::TrackedDeviceIndex_t p_tracked_device_index, uint32_t pMaxLen) {
	static char returnstring[1025] = "Not initialised";

	if (hmd != NULL) {
		// don't go bigger then this...
		if (pMaxLen > 1024) {
			pMaxLen = 1024;
		}

		if ((hmd != NULL) && (p_tracked_device_index != vr::k_unTrackedDeviceIndexInvalid)) {
			uint32_t namelength = hmd->GetStringTrackedDeviceProperty(p_tracked_device_index, vr::Prop_RenderModelName_String, NULL, 0, NULL);
			if (namelength > 0) {
				if (namelength > pMaxLen) {
					namelength = pMaxLen;
				};

				hmd->GetStringTrackedDeviceProperty(p_tracked_device_index, vr::Prop_RenderModelName_String, returnstring, namelength, NULL);
			}
		}
	}

	return returnstring;
}

int32_t openvr_data::get_controller_role(vr::TrackedDeviceIndex_t p_tracked_device_index) {
	vr::ETrackedPropertyError error;

	if (hmd == NULL) {
		return 0;
	}

	int32_t controllerRole = hmd->GetInt32TrackedDeviceProperty(p_tracked_device_index, vr::Prop_ControllerRoleHint_Int32, &error);
	// should add error handling

	return controllerRole;
}

bool openvr_data::is_tracked_device_connected(vr::TrackedDeviceIndex_t p_tracked_device_index) {
	if (hmd == NULL) {
		return false;
	}

	return hmd->IsTrackedDeviceConnected(p_tracked_device_index);
}

vr::TrackedDeviceClass openvr_data::get_tracked_device_class(vr::TrackedDeviceIndex_t p_tracked_device_index) {
	if (hmd == NULL) {
		return vr::TrackedDeviceClass_Invalid;
	}

	return hmd->GetTrackedDeviceClass(p_tracked_device_index);
}

Transform3D openvr_data::get_eye_to_head_transform(int p_view, float p_world_scale) {
	if (hmd == NULL) {
		return Transform3D();
	}

	vr::HmdMatrix34_t matrix = hmd->GetEyeToHeadTransform(p_view == 0 ? vr::Eye_Left : vr::Eye_Right);

	return transform_from_matrix(&matrix, p_world_scale);
}

////////////////////////////////////////////////////////////////
// Interact with tracking info

////////////////////////////////////////////////////////////////
// Register an action set
// Note that we can't remove action sets once added so our index
// shouldn't change
int openvr_data::register_action_set(const String p_action_set) {
	for (int i = 0; i < action_sets.size(); i++) {
		if (action_sets[i].name == p_action_set) {
			return i;
		}
	}

	action_set new_action_set;
	new_action_set.name = p_action_set;
	new_action_set.handle = vr::k_ulInvalidActionSetHandle;
	new_action_set.is_active = false;

	if (is_initialised()) {
		vr::EVRInputError err = vr::VRInput()->GetActionSetHandle((const char *)new_action_set.name, &new_action_set.handle);
		if (err != vr::VRInputError_None) {
			Utilities::print(String("Failed to obtain action set handle for ") + new_action_set.name);
		}
	}

	action_sets.push_back(new_action_set);

	return (int)action_sets.size() - 1;
}

////////////////////////////////////////////////////////////////
// Set the active action set
void openvr_data::set_active_action_set(const String p_action_set) {
	bool found = false;
	for (int i = 0; i < action_sets.size(); i++) {
		if (action_sets[i].name == p_action_set) {
			// found it!
			action_sets[i].is_active = true;
			found = true;
		} else {
			action_sets[i].is_active = false;
		}
	}

	// couldn't find it?? Make our default active
	if (found) {
		if (action_sets.size() > 0) {
			action_sets[0].is_active = true;
		}
	}

	active_action_set_count = 1;
}

void openvr_data::toggle_action_set_active(const String p_action_set, const bool p_is_active) {
	for (int i = 0; i < action_sets.size(); i++) {
		if (action_sets[i].name == p_action_set) {
			// found it!
			if (action_sets[i].is_active != p_is_active) {
				action_sets[i].is_active = p_is_active;
				if (p_is_active) {
					active_action_set_count++;
				} else {
					active_action_set_count--;
				}
			}
			return;
		}
	}
}

bool openvr_data::is_action_set_active(const String p_action_set) const {
	for (int i = 0; i < action_sets.size(); i++) {
		if (action_sets[i].name == p_action_set) {
			// found it!
			return action_sets[i].is_active;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////
// Bind our default actions, if the actions file has been edit
// and the defaults removed they will simply be undefined
void openvr_data::bind_default_action_handles() {
	const char *input_actions[DAH_IN_MAX] = {
		"trigger",
		"analog_trigger",
		"grip",
		"analog_grip",
		"analog",
		"analog_click",
		"button_ax",
		"button_by"
	};
	const char *output_actions[DAH_OUT_MAX] = {
		"haptic"
	};

	for (int i = 0; i < DAH_IN_MAX; i++) {
		char action_path[1024];
		sprintf(action_path, "%s/in/%s", (const char *)action_sets[0].name, input_actions[i]);

		input_action_handles[i] = vr::k_ulInvalidActionHandle;

		vr::EVRInputError err = vr::VRInput()->GetActionHandle(action_path, &input_action_handles[i]);
		if (err == vr::VRInputError_None) {
			Utilities::print(String("Bound action {0} to {1}").format(Array::make(String(action_path), Variant((int64_t)input_action_handles[i]))));
		} else {
			Utilities::print(String("Failed to bind action {0}, error code: {1}").format(Array::make(String(action_path), Variant((int64_t)err))));
		}
	}

	for (int i = 0; i < DAH_OUT_MAX; i++) {
		char action_path[1024];
		sprintf(action_path, "%s/out/%s", (const char *)action_sets[0].name, output_actions[i]);

		output_action_handles[i] = vr::k_ulInvalidActionHandle;

		vr::EVRInputError err = vr::VRInput()->GetActionHandle(action_path, &output_action_handles[i]);
		if (err == vr::VRInputError_None) {
			Utilities::print(String("Bound action {0} to {1}").format(Array::make(String(action_path), Variant((int64_t)output_action_handles[i]))));
		} else {
			Utilities::print(String("Failed to bind action {0}, error code: {1}").format(Array::make(String(action_path), Variant((int64_t)err))));
		}
	}
}

////////////////////////////////////////////////////////////////
// Register a custom action
// Note that we can't remove actions once added so our index
// shouldn't change
int openvr_data::register_custom_action(const String p_action) {
	// first find if we already have this
	// TODO: we should contemplate changing this to a dictionary, but I think this array will remain small enough
	for (int i = 0; i < custom_actions.size(); i++) {
		if (custom_actions[i].name == p_action) {
			// found it
			return i;
		}
	}

	// ok this one is new, lets add it
	custom_action new_action;
	new_action.name = p_action;
	new_action.handle = vr::k_ulInvalidActionHandle;

	if (is_initialised()) {
		vr::EVRInputError err = vr::VRInput()->GetActionHandle((const char *)new_action.name, &new_action.handle);
		if (err == vr::VRInputError_None) {
			Utilities::print(String("Bound action {0} to {1}").format(Array::make(new_action.name, Variant((int64_t)new_action.handle))));
		} else {
			Utilities::print(String("Failed to obtain action handle for {0}").format(Array::make(new_action.name)));
		}
	}

	custom_actions.push_back(new_action);
	return (int)custom_actions.size() - 1;
}

vr::VRActionHandle_t openvr_data::get_custom_handle(int p_action_idx) {
	if (p_action_idx < 0) {
		// we never registered our handle
		// printf("Index not setup: %i\n", p_action_idx);
		return vr::k_ulInvalidActionHandle;
	} else if (p_action_idx >= custom_actions.size()) {
		// index out of bounds
		// printf("Index out of bounds: %i\n", p_action_idx);
		return vr::k_ulInvalidActionHandle;
	} else {
		return custom_actions[p_action_idx].handle;
	}
}

bool openvr_data::get_custom_pose_data(int p_action_idx, vr::InputPoseActionData_t *p_data, int p_on_hand) {
	if (p_action_idx < 0) {
		// we never registered our handle
		// printf("Index not setup: %i\n", p_action_idx);
		return false;
	} else if (p_action_idx >= custom_actions.size()) {
		// index out of bounds
		// printf("Index out of bounds: %i\n", p_action_idx);
		return false;
	} else if (custom_actions[p_action_idx].handle == vr::k_ulInvalidActionHandle) {
		// action doesn't exist
		// printf("No handle set for action: %s\n", custom_actions[p_action_idx].name);
		return false;
	}

	vr::VRInputValueHandle_t source_handle = vr::k_ulInvalidInputValueHandle;
	if (p_on_hand == 1 && left_hand_device != vr::k_unTrackedDeviceIndexInvalid) {
		source_handle = tracked_devices[left_hand_device].source_handle;
	} else if (p_on_hand == 2 && right_hand_device != vr::k_unTrackedDeviceIndexInvalid) {
		source_handle = tracked_devices[right_hand_device].source_handle;
	}

	// let's retrieve it
	vr::EVRInputError err = vr::VRInput()->GetPoseActionDataForNextFrame(custom_actions[p_action_idx].handle, vr::TrackingUniverseStanding, p_data, sizeof(vr::InputPoseActionData_t), source_handle);
	if (err != vr::VRInputError_None) {
		// printf("Couldn't retrieve pose %i\n", err);
		return false;
	}

	return true;
}

bool openvr_data::get_custom_digital_data(int p_action_idx, int p_on_hand) {
	if (p_action_idx < 0) {
		// we never registered our handle
		// printf("Index not setup: %i\n", p_action_idx);
		return false;
	} else if (p_action_idx >= custom_actions.size()) {
		// index out of bounds
		// printf("Index out of bounds: %i\n", p_action_idx);
		return false;
	} else if (custom_actions[p_action_idx].handle == vr::k_ulInvalidActionHandle) {
		// action doesn't exist
		// printf("No handle set for action: %s\n", custom_actions[p_action_idx].name);
		return false;
	}

	vr::VRInputValueHandle_t source_handle = vr::k_ulInvalidInputValueHandle;
	if (p_on_hand == 1 && left_hand_device != vr::k_unTrackedDeviceIndexInvalid) {
		source_handle = tracked_devices[left_hand_device].source_handle;
	} else if (p_on_hand == 2 && right_hand_device != vr::k_unTrackedDeviceIndexInvalid) {
		source_handle = tracked_devices[right_hand_device].source_handle;
	}

	// let's retrieve it
	vr::InputDigitalActionData_t digital_data;
	vr::EVRInputError err = vr::VRInput()->GetDigitalActionData(custom_actions[p_action_idx].handle, &digital_data, sizeof(digital_data), source_handle);
	if (err != vr::VRInputError_None) {
		// printf("Couldn't retrieve digital %i\n", err);
		return false;
	}

	return digital_data.bActive && digital_data.bState;
}

godot::Vector2 openvr_data::get_custom_analog_data(int p_action_idx, int p_on_hand) {
	if (p_action_idx < 0) {
		// we never registered our handle
		// printf("Index not setup: %i\n", p_action_idx);
		return godot::Vector2();
	} else if (p_action_idx >= custom_actions.size()) {
		// index out of bounds
		// printf("Index out of bounds: %i\n", p_action_idx);
		return godot::Vector2();
	} else if (custom_actions[p_action_idx].handle == vr::k_ulInvalidActionHandle) {
		// action doesn't exist
		// printf("No handle set for action: %s\n", custom_actions[p_action_idx].name);
		return godot::Vector2();
	}

	vr::VRInputValueHandle_t source_handle = vr::k_ulInvalidInputValueHandle;
	if (p_on_hand == 1 && left_hand_device != vr::k_unTrackedDeviceIndexInvalid) {
		source_handle = tracked_devices[left_hand_device].source_handle;
	} else if (p_on_hand == 2 && right_hand_device != vr::k_unTrackedDeviceIndexInvalid) {
		source_handle = tracked_devices[right_hand_device].source_handle;
	}

	// let's retrieve it
	vr::InputAnalogActionData_t analog_data;
	vr::EVRInputError err = vr::VRInput()->GetAnalogActionData(custom_actions[p_action_idx].handle, &analog_data, sizeof(analog_data), source_handle);
	if (err != vr::VRInputError_None) {
		// printf("Couldn't retrieve analog %i\n", err);
		return godot::Vector2();
	} else if (!analog_data.bActive) {
		return godot::Vector2();
	}

	return godot::Vector2(analog_data.x, analog_data.y);
}

bool openvr_data::trigger_custom_haptic(int p_action_idx, float p_start_from_now, float p_duration, float p_frequency, float p_amplitude, int p_on_hand) {
	if (p_action_idx < 0) {
		// we never registered our handle
		// printf("Index not setup: %i\n", p_action_idx);
		return false;
	} else if (p_action_idx >= custom_actions.size()) {
		// index out of bounds
		// printf("Index out of bounds: %i\n", p_action_idx);
		return false;
	} else if (custom_actions[p_action_idx].handle == vr::k_ulInvalidActionHandle) {
		// action doesn't exist
		// printf("No handle set for action: %s\n", custom_actions[p_action_idx].name);
		return false;
	}

	vr::VRInputValueHandle_t source_handle = vr::k_ulInvalidInputValueHandle;
	if (p_on_hand == 1 && left_hand_device != vr::k_unTrackedDeviceIndexInvalid) {
		source_handle = tracked_devices[left_hand_device].source_handle;
	} else if (p_on_hand == 2 && right_hand_device != vr::k_unTrackedDeviceIndexInvalid) {
		source_handle = tracked_devices[right_hand_device].source_handle;
	}

	vr::VRInput()->TriggerHapticVibrationAction(custom_actions[p_action_idx].handle, p_start_from_now, p_duration, p_frequency, p_amplitude, source_handle);

	return true;
}

////////////////////////////////////////////////////////////////
// Called when we detect a new device, set it up
void openvr_data::attach_device(uint32_t p_device_index) {
	tracked_device *device = &tracked_devices[p_device_index];

	if (p_device_index == vr::k_unTrackedDeviceIndexInvalid) {
		// really?!
	} else if (device->tracker_id == 0) {
		char device_name[256];
		strcpy(device_name, get_device_name(p_device_index, 255));

		vr::TrackedDeviceClass device_class = get_tracked_device_class(p_device_index);
		if (device_class == vr::TrackedDeviceClass_TrackingReference) {
			// ignore base stations and cameras for now
			Utilities::print(String("Found base station {0} ({1})").format(Array::make(Variant((int64_t)p_device_index), String(device_name))));
		} else if (device_class == vr::TrackedDeviceClass_HMD) {
			// ignore any HMD
			Utilities::print(String("Found HMD {0} ({1})").format(Array::make(Variant((int64_t)p_device_index), String(device_name))));
		} else {
			godot_int hand = 0;

			if (device_class == vr::TrackedDeviceClass_Controller) {
				Utilities::print(String("Found controller {0} ({1})").format(Array::make(Variant((int64_t)p_device_index), String(device_name))));

				// If this is a controller than get our controller role
				int32_t controllerRole = get_controller_role(p_device_index);
				if (controllerRole == vr::TrackedControllerRole_RightHand) {
					hand = 2;
					device_hands_are_available = true;
				} else if (controllerRole == vr::TrackedControllerRole_LeftHand) {
					hand = 1;
					device_hands_are_available = true;
				} else if (!device_hands_are_available) {
					// this definately needs to improve, if we haven't got hand information, our first controller becomes left and our second becomes right
					if (left_hand_device == vr::k_unTrackedDeviceIndexInvalid) {
						hand = 1;
					} else if (right_hand_device == vr::k_unTrackedDeviceIndexInvalid) {
						hand = 2;
					}
				}
			} else {
				Utilities::print(String("Found tracker {0} ({1})").format(Array::make(Variant((int64_t)p_device_index), String(device_name))));
			}

			sprintf(&device_name[strlen(device_name)], "_%i", p_device_index);
			device->tracker_id = godot::xr_api->godot_xr_add_controller(device_name, hand, true, true);

			// remember our primary left and right hand devices
			if ((hand == 1) && (left_hand_device == vr::k_unTrackedDeviceIndexInvalid)) {
				vr::VRInput()->GetInputSourceHandle("/user/hand/left", &device->source_handle);
				left_hand_device = p_device_index;
			} else if ((hand == 2) && (right_hand_device == vr::k_unTrackedDeviceIndexInvalid)) {
				vr::VRInput()->GetInputSourceHandle("/user/hand/right", &device->source_handle);
				right_hand_device = p_device_index;
			} else {
				// other devices don't have source handles...
				device->source_handle = vr::k_ulInvalidInputValueHandle;
			}
		}
	}
}

////////////////////////////////////////////////////////////////
// Called when we loose tracked device, cleanup
void openvr_data::detach_device(uint32_t p_device_index) {
	if (p_device_index == vr::k_unTrackedDeviceIndexInvalid) {
		// really?!
	} else if (tracked_devices[p_device_index].tracker_id != 0) {
		godot::xr_api->godot_xr_remove_controller(tracked_devices[p_device_index].tracker_id);
		tracked_devices[p_device_index].tracker_id = 0;

		// unset left/right hand devices
		if (left_hand_device == p_device_index) {
			left_hand_device = vr::k_unTrackedDeviceIndexInvalid;
		} else if (right_hand_device == p_device_index) {
			right_hand_device = vr::k_unTrackedDeviceIndexInvalid;
		}
	}
}

////////////////////////////////////////////////////////////////
// Called by our process loop to handle our fixed actions
void openvr_data::process_device_actions(tracked_device *p_device, uint64_t p_msec) {
	const char input_types[DAH_IN_MAX] = { 'b', 'f', 'b', 'f', 'v', 'b', 'b', 'b' }; // input type (b)oolean, (f)loat, (v)ector2
	const int godot_index[DAH_IN_MAX] = { 15, 2, 2, 4, 0, 14, 7, 1 }; // button / axis indexes

	if (p_device->source_handle == vr::k_ulInvalidInputValueHandle) {
		return;
	}

	// look through our inputs...
	for (int i = 0; i < DAH_IN_MAX; i++) {
		if (input_action_handles[i] != vr::k_ulInvalidActionHandle) {
			switch (input_types[i]) {
				case 'b': {
					vr::InputDigitalActionData_t action_data;
					vr::EVRInputError err = vr::VRInput()->GetDigitalActionData(input_action_handles[i], &action_data, sizeof(action_data), p_device->source_handle);
					if (err == vr::VRInputError_None) {
						godot::xr_api->godot_xr_set_controller_button(p_device->tracker_id, godot_index[i], action_data.bActive && action_data.bState);
					}
				} break;
				case 'f': { // vector1
					vr::InputAnalogActionData_t analog_data;
					vr::EVRInputError err = vr::VRInput()->GetAnalogActionData(input_action_handles[i], &analog_data, sizeof(analog_data), p_device->source_handle);
					if (err == vr::VRInputError_None) {
						if (analog_data.bActive) {
							godot::xr_api->godot_xr_set_controller_axis(
									p_device->tracker_id, godot_index[i], analog_data.x, true);
						}
					}
				} break;
				case 'v': { // vector2
					vr::InputAnalogActionData_t analog_data;
					vr::EVRInputError err = vr::VRInput()->GetAnalogActionData(input_action_handles[i], &analog_data, sizeof(analog_data), p_device->source_handle);
					if (err == vr::VRInputError_None) {
						if (analog_data.bActive) {
							godot::xr_api->godot_xr_set_controller_axis(
									p_device->tracker_id, godot_index[i], analog_data.x, true);
							godot::xr_api->godot_xr_set_controller_axis(
									p_device->tracker_id, godot_index[i] + 1, analog_data.y, true);
						}
					}
				} break;
				default: {
					// ????
				} break;
			}
		}
	}

	// and check our haptic output
	if (output_action_handles[DAH_OUT_HAPTIC] != vr::k_ulInvalidActionHandle) {
		float rumble = (float)godot::xr_api->godot_xr_get_controller_rumble(p_device->tracker_id);
		if ((rumble > 0.0) && ((p_msec - p_device->last_rumble_update) > 100)) {
			// We should only call this once ever 100ms...
			vr::VRInput()->TriggerHapticVibrationAction(output_action_handles[DAH_OUT_HAPTIC], 0, 0.1f, 4.f, rumble, p_device->source_handle);
			p_device->last_rumble_update = p_msec;
		}
	}
}

////////////////////////////////////////////////////////////////
// Get the name of our default action set
String openvr_data::get_default_action_set() const {
	return action_sets[0].name;
}

////////////////////////////////////////////////////////////////
// Set the name of our default action set
void openvr_data::set_default_action_set(const String p_name) {
	if (is_initialised()) {
		Utilities::print("OpenVR has already been initialised");
	} else {
		action_sets[0].name = p_name;
	}
}

const godot::Transform3D openvr_data::get_hmd_transform() const {
	return hmd_transform;
}

////////////////////////////////////////////////////////////////
// Interact with render models

////////////////////////////////////////////////////////////////
// Return the number of render models we have access too
uint32_t openvr_data::get_render_model_count() {
	if (hmd == NULL) {
		return 0;
	}

	return render_models->GetRenderModelCount();
}

////////////////////////////////////////////////////////////////
// get the name of a render model at a given index
godot::String openvr_data::get_render_model_name(uint32_t p_model_index) {
	godot::String s;

	if (hmd != NULL) {
		char model_name[256];
		render_models->GetRenderModelName(p_model_index, model_name, 256);
		s = model_name;
	}

	return s;
}

////////////////////////////////////////////////////////////////
// load the given render model into the provided ArrayMesh
void openvr_data::load_render_model(const String &p_model_name, ArrayMesh *p_mesh) {
	// if we already have an entry, remove it
	remove_mesh(p_mesh);

	// add an entry, we'll attempt a load
	model_mesh new_entry;

	strcpy(new_entry.model_name, p_model_name);
	new_entry.mesh = p_mesh;

	load_models.push_back(new_entry);
}

////////////////////////////////////////////////////////////////
// Actually load our model this is called from our process so
// we're in our render thread.
// OpenVR loads this in a separate thread so we repeatedly call this
// until the model is loaded and only then process it
bool openvr_data::_load_render_model(model_mesh *p_model) {
	vr::RenderModel_t *ovr_render_model = NULL;

	// Load our render model
	vr::EVRRenderModelError err = render_models->LoadRenderModel_Async(p_model->model_name, &ovr_render_model);
	if (err == vr::VRRenderModelError_Loading) {
		// still loading.. check again later
		return false;
	}

	if (err != vr::VRRenderModelError_None) {
		Utilities::print(String("OpenVR: Couldn''t find model for ") + String(p_model->model_name) + " (" + String((int64_t)err) + ")");

		// don't try again, remove it from our list
		return true;
	}

	PackedVector3Array vertices;
	PackedVector3Array normals;
	PackedVector2Array texcoords;
	PackedInt32Array indices;
	Array arr;
	Array blend_array;

	// resize our arrays
	vertices.resize(ovr_render_model->unVertexCount);
	normals.resize(ovr_render_model->unVertexCount);
	texcoords.resize(ovr_render_model->unVertexCount);
	indices.resize(ovr_render_model->unTriangleCount * 3);

	// copy our vertices
	{
		// lock for writing
		// PoolVector3Array::Write vw = vertices.write();
		// PoolVector3Array::Write nw = normals.write();
		// PoolVector2Array::Write tw = texcoords.write();

		for (uint32_t i = 0; i < ovr_render_model->unVertexCount; i++) {
			// vw[i] = Vector3(ovr_render_model->rVertexData[i].vPosition.v[0], ovr_render_model->rVertexData[i].vPosition.v[1], ovr_render_model->rVertexData[i].vPosition.v[2]);
			// nw[i] = Vector3(ovr_render_model->rVertexData[i].vNormal.v[0], ovr_render_model->rVertexData[i].vNormal.v[1], ovr_render_model->rVertexData[i].vNormal.v[2]);
			// tw[i] = Vector2(ovr_render_model->rVertexData[i].rfTextureCoord[0], ovr_render_model->rVertexData[i].rfTextureCoord[1]);

			vertices[i] = Vector3(ovr_render_model->rVertexData[i].vPosition.v[0], ovr_render_model->rVertexData[i].vPosition.v[1], ovr_render_model->rVertexData[i].vPosition.v[2]);
			normals[i] = Vector3(ovr_render_model->rVertexData[i].vNormal.v[0], ovr_render_model->rVertexData[i].vNormal.v[1], ovr_render_model->rVertexData[i].vNormal.v[2]);
			texcoords[i] = Vector2(ovr_render_model->rVertexData[i].rfTextureCoord[0], ovr_render_model->rVertexData[i].rfTextureCoord[1]);
		}
	}

	// copy our indices, for some reason these are other way around :)
	{
		// lock for writing
		// PoolIntArray::Write iw = indices.write();

		for (uint32_t i = 0; i < ovr_render_model->unTriangleCount * 3; i += 3) {
			// iw[i + 0] = ovr_render_model->rIndexData[i + 2];
			// iw[i + 1] = ovr_render_model->rIndexData[i + 1];
			// iw[i + 2] = ovr_render_model->rIndexData[i + 0];

			indices[i + 0] = ovr_render_model->rIndexData[i + 2];
			indices[i + 1] = ovr_render_model->rIndexData[i + 1];
			indices[i + 2] = ovr_render_model->rIndexData[i + 0];
		}
	}

	// create our array for our model
	arr.resize(ArrayMesh::ARRAY_MAX);

	// load our pool arrays into our array
	arr[ArrayMesh::ARRAY_VERTEX] = vertices;
	arr[ArrayMesh::ARRAY_NORMAL] = normals;
	arr[ArrayMesh::ARRAY_TEX_UV] = texcoords;
	arr[ArrayMesh::ARRAY_INDEX] = indices;

	// and load
	p_model->mesh->add_surface_from_arrays(ArrayMesh::PRIMITIVE_TRIANGLES, arr, blend_array);

	// prepare our material
	Ref<StandardMaterial3D> material;
	material.instance();

	// queue loading our textures
	load_texture(TT_ALBEDO, ovr_render_model->diffuseTextureId, material);

	// assign material to our model
	p_model->mesh->surface_set_material(0, material);

	// free up our render model
	render_models->FreeRenderModel(ovr_render_model);

	// I guess we're done...
	return true;
}

////////////////////////////////////////////////////////////////
// Load a texture and load it into a standard material
void openvr_data::load_texture(TextureType p_type, vr::TextureID_t p_texture_id, Ref<StandardMaterial3D> p_material) {
	// add an entry, we'll attempt a load
	texture_material new_entry;

	new_entry.type = p_type;
	new_entry.texture_id = p_texture_id;
	new_entry.material = p_material;

	load_textures.push_back(new_entry);
}

////////////////////////////////////////////////////////////////
// Performs the actual load of a texture, called from our
// render thread.
// OpenVR loads this in a separate thread so we repeatedly call this
// until the texture is loaded and only then process it
bool openvr_data::_load_texture(texture_material *p_texture) {
	vr::RenderModel_TextureMap_t *ovr_texture = NULL;

	// load our texture
	vr::EVRRenderModelError err = vr::VRRenderModels()->LoadTexture_Async(p_texture->texture_id, &ovr_texture);
	if (err == vr::VRRenderModelError_Loading) {
		// still loading.. check again later
		return false;
	}

	if (err != vr::VRRenderModelError_None) {
		Utilities::print(String("OpenVR: Couldn''t find texture for ") + String((int64_t)p_texture->texture_id) + " (" + String((int64_t)err) + ")");

		// reset our references to ensure our material gets freed at the right time
		p_texture->material = Ref<StandardMaterial3D>();

		// don't try again, remove it from our list
		return true;
	}

	PackedByteArray image_data;
	image_data.resize(ovr_texture->unWidth * ovr_texture->unHeight * 4);

	{
		// Need to improve this, this is sloooooow...
		// PoolByteArray::Write idw = image_data.write();
		// memcpy(idw.ptr(), ovr_texture->rubTextureMapData, ovr_texture->unWidth * ovr_texture->unHeight * 4);
		for (int i = 0; i < ovr_texture->unWidth * ovr_texture->unHeight * 4; i++) {
			image_data[i] = ovr_texture->rubTextureMapData[i];
		}
	}

	Ref<Image> image;
	image.instance();
	image->create_from_data(ovr_texture->unWidth, ovr_texture->unHeight, false, Image::FORMAT_RGBA8, image_data);

	Ref<ImageTexture> texture;
	texture.instance();
	texture->create_from_image(image);

	switch (p_texture->type) {
		case TT_ALBEDO:
			p_texture->material->set_texture(StandardMaterial3D::TEXTURE_ALBEDO, texture);
			break;
		default: break;
	}

	// reset our references to ensure our material gets freed at the right time
	p_texture->material = Ref<StandardMaterial3D>();

	// I guess we're done...
	return true;
}

////////////////////////////////////////////////////////////////
// Remove our mesh from our load queue
void openvr_data::remove_mesh(ArrayMesh *p_mesh) {
	// check in reverse so we can safely remove things
	for (int i = (int)load_models.size() - 1; i >= 0; i--) {
		if (load_models[i].mesh == p_mesh) {
			load_models.erase(load_models.begin() + i);
		}
	}
}

////////////////////////////////////////////////////////////////
// Convert a matrix we get from OpenVR into the format Godot
// requires
Transform3D openvr_data::transform_from_matrix(vr::HmdMatrix34_t *p_matrix, godot_float p_world_scale) {
	Transform3D ret;

	float *basis_ptr = (float *)&ret.basis.elements;
	int k = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			basis_ptr[k++] = p_matrix->m[i][j];
		}
	}

	ret.origin.x = (real_t)(p_matrix->m[0][3] * p_world_scale);
	ret.origin.y = (real_t)(p_matrix->m[1][3] * p_world_scale);
	ret.origin.z = (real_t)(p_matrix->m[2][3] * p_world_scale);

	return ret;
}

void openvr_data::transform_from_matrix(godot_transform3d *p_dest, vr::HmdMatrix34_t *p_matrix, godot_float p_world_scale) {
	Transform3D *dest = (Transform3D *)p_dest;
	float *basis_ptr = (float *)&dest->basis; // Godot can switch between real_t being
	// double or float.. which one is used...

	int k = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			basis_ptr[k++] = p_matrix->m[i][j];
		}
	}

	dest->origin.x = (real_t)(p_matrix->m[0][3] * p_world_scale);
	dest->origin.y = (real_t)(p_matrix->m[1][3] * p_world_scale);
	dest->origin.z = (real_t)(p_matrix->m[2][3] * p_world_scale);
}

////////////////////////////////////////////////////////////////
// Convert a transform from Godot into a matrix OpenVR requires
void openvr_data::matrix_from_transform(vr::HmdMatrix34_t *p_matrix, godot_transform3d *p_transform, godot_float p_world_scale) {
	godot::Transform3D *transform = (godot::Transform3D *)p_transform;

	p_matrix->m[0][3] = transform->origin.x / (real_t)p_world_scale;
	p_matrix->m[1][3] = transform->origin.y / (real_t)p_world_scale;
	p_matrix->m[2][3] = transform->origin.z / (real_t)p_world_scale;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			p_matrix->m[i][j] = transform->basis[j][i];
		}
	}
}

////////////////////////////////////////////////////////////////
// Convert a bone transform we get from OpenVR into a Godot
// transform
void openvr_data::transform_from_bone(Transform3D &p_transform, const vr::VRBoneTransform_t *p_bone_transform) {
	// OpenVR uses quaternions which is so much better for bones but Godot uses matrices.. so convert back...
	Quaternion q(p_bone_transform->orientation.x, p_bone_transform->orientation.y, p_bone_transform->orientation.z, p_bone_transform->orientation.w);

	p_transform.basis = Basis(q);
	p_transform.origin = Vector3(p_bone_transform->position.v[0], p_bone_transform->position.v[1], p_bone_transform->position.v[2]);
}
