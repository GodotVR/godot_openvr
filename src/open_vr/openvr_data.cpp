////////////////////////////////////////////////////////////////////////////////////////////////
// Helper calls and singleton container for accessing openvr

#include "openvr_data.h"

#include "godot_cpp/classes/time.hpp"
#include "godot_cpp/classes/xr_server.hpp"
#include <godot_cpp/variant/utility_functions.hpp>

#include <string.h>

using namespace godot;

openvr_data *openvr_data::singleton = nullptr;

openvr_data::openvr_data() {
	// get some default values
	use_count = 1;
	hmd = nullptr;
	render_models = nullptr;

	application_type = OpenVRApplicationType::SCENE;
	tracking_universe = OpenVRTrackingUniverse::STANDING;

	chaperone = nullptr;
	play_area_is_dirty = true;
	for (int i = 0; i < 4; i++) {
		play_area[i].x = 0.0f;
		play_area[i].y = 0.0f;
		play_area[i].z = 0.0f;
	}

	// setting up our action set data
	// TODO we should find a way to read this from our json

	int default_action_set = register_action_set(String("/actions/godot"));
	action_sets[default_action_set].is_active = true;
	active_action_set_count = 1;

	// Our default actions, we should be loading this from our action json

	// TODO rename actions so this is 1:1
	add_input_action("primary", "primary", IT_VECTOR2);
	add_input_action("secondary", "secondary", IT_VECTOR2);
	add_input_action("trigger_value", "analog_trigger", IT_FLOAT);
	add_input_action("grip_value", "analog_grip", IT_FLOAT);

	add_input_action("primary_click", "primary_click", IT_BOOL);
	add_input_action("secondary_click", "secondary_click", IT_BOOL);
	add_input_action("trigger_click", "trigger", IT_BOOL);
	add_input_action("grip_click", "grip", IT_BOOL);
	add_input_action("ax", "button_ax", IT_BOOL);
	add_input_action("by", "button_by", IT_BOOL);
	// add_input_action("menu", "???", IT_BOOL);

	// here we remove the _pose or we'll overlap action names but we don't want the _pose in Godot
	add_pose_action("aim", "aim_pose");
	add_pose_action("grip", "grip_pose");
}

openvr_data::~openvr_data() {
	cleanup();

	if (singleton == this) {
		singleton = nullptr;
	}
}

void openvr_data::cleanup() {
	if (hmd != nullptr) {
		// reset our action handles
		for (auto &input : inputs) {
			input.handle = vr::k_ulInvalidActionHandle;
		}

		for (auto &pose : poses) {
			pose.handle = vr::k_ulInvalidActionHandle;
		}

		// detach all our devices
		for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
			detach_device(i);
		}

		if (head_tracker.is_valid()) {
			XRServer *xr_server = XRServer::get_singleton();
			if (xr_server != nullptr) {
				xr_server->remove_tracker(head_tracker);
			}

			head_tracker.unref();
		}

		// forget our custom actions

		// forget our action sets
		for (std::vector<action_set>::iterator it = action_sets.begin(); it != action_sets.end(); ++it) {
			it->handle = vr::k_ulInvalidActionSetHandle;
		}

		hmd = nullptr;
		render_models = nullptr;
		chaperone = nullptr;

		vr::VR_Shutdown();
	}
}

openvr_data *openvr_data::retain_singleton() {
	if (singleton == nullptr) {
		singleton = new openvr_data();
	} else {
		singleton->use_count++;

		String msg("Usage count increased to {0}");
		Array arr;
		arr.push_back(Variant(singleton->use_count));

		UtilityFunctions::print(msg.format(arr));
	}
	return singleton;
}

void openvr_data::release() {
	if (singleton != this) {
		// this should never happen!
		UtilityFunctions::print("openvr object does not match singleton!");
	} else if (singleton->use_count > 1) {
		singleton->use_count--;

		String msg("Usage count decreased to {0}");
		Array arr;
		arr.push_back(Variant(singleton->use_count));

		UtilityFunctions::print(msg.format(arr));
	} else {
		delete this;
	}
}

bool openvr_data::is_initialised() {
	return hmd != nullptr;
}

bool openvr_data::initialise() {
	if (hmd != nullptr) {
		// already initialised, no need to do again
		return true;
	}

	// init openvr
	UtilityFunctions::print("OpenVR: initialising OpenVR context\n");

	bool success = true;
	vr::EVRInitError error = vr::VRInitError_None;

	if (!vr::VR_IsRuntimeInstalled()) {
		UtilityFunctions::print("SteamVR has not been installed.");
		success = false;
	}

	if (success) {
		// Loading the SteamVR Runtime
		if (application_type == OpenVRApplicationType::OVERLAY) {
			hmd = vr::VR_Init(&error, vr::VRApplication_Overlay);
			UtilityFunctions::print("Application in overlay mode.");
		} else {
			hmd = vr::VR_Init(&error, vr::VRApplication_Scene);
			UtilityFunctions::print("Application in scene (normal) mode.");
		}

		if (error != vr::VRInitError_None) {
			success = false;

			Array arr;
			arr.push_back(String(vr::VR_GetVRInitErrorAsEnglishDescription(error)));
			UtilityFunctions::print(String("Unable to init VR runtime: {0}").format(arr));
		} else {
			UtilityFunctions::print("Main OpenVR interface has been initialized");
		}
	}

	if (success) {
		// render models give us access to mesh representations of the various
		// controllers
		render_models = (vr::IVRRenderModels *)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &error);
		if (!render_models) {
			success = false;

			Array arr;
			arr.push_back(String(vr::VR_GetVRInitErrorAsEnglishDescription(error)));
			UtilityFunctions::print(String("Unable to get render model interface: {0}").format(arr));
		} else {
			UtilityFunctions::print("Main render models interface has been initialized.");
		}
	}

	if (success) {
		if (!vr::VRCompositor()) {
			success = false;

			UtilityFunctions::print("Compositor initialization failed. See log file for details.");
		}
	}

	if (success) {
		if (application_type == OpenVRApplicationType::OVERLAY) {
			if (!vr::VROverlay()) {
				success = false;

				UtilityFunctions::print("Overlay system initialization failed. See log file for details.");
			}
		}
	}

	if (success) {
		chaperone = vr::VRChaperone();
		if (chaperone == nullptr) {
			success = false;

			UtilityFunctions::print("Chaperone initialization failed. See log file for details.");
		} else {
			update_play_area();
		}
	}

	if (success) {
		OS *os = OS::get_singleton();
		ProjectSettings *project_settings = ProjectSettings::get_singleton();
		Ref<DirAccess> directory = DirAccess::open("res://");

		String exec_path = os->get_executable_path().replace("\\", "/").get_base_dir();
		String manifest_path;

		// check 3 locations in order
		// 1) check if we have an action folder alongside our executable (runtime deployed actions)
		String path = exec_path.path_join("actions/actions.json");
		if (directory->file_exists(path)) {
			manifest_path = path;
		} else {
			// 2) else check if we have an action folder in our project folder (custom user actions in development)
			path = "res://actions/actions.json";
			if (directory->file_exists(path)) {
				manifest_path = path;
			} else {
				// 3) else check if we have an action folder in our plugin (if no user overrule)
				path = "res://addons/godot-openvr/actions/actions.json";
				if (directory->file_exists(path)) {
					manifest_path = path;
				}
			}
		}

		if (manifest_path.length() != 0) {
			String absolute_path;
			if (os->has_feature("editor")) {
				absolute_path = project_settings->globalize_path(manifest_path);
			} else {
				absolute_path = exec_path.path_join(manifest_path);
			}

			vr::EVRInputError err = vr::VRInput()->SetActionManifestPath(absolute_path.utf8().get_data());
			if (err == vr::VRInputError_None) {
				Array arr;
				arr.push_back(manifest_path);
				UtilityFunctions::print(String("Loaded action json from: {0}").format(arr));
			} else {
				success = false;
				Array arr;
				arr.push_back(manifest_path);
				UtilityFunctions::print(String("Failed to load action json from: {0}").format(arr));
			}
		} else {
			success = false;
			UtilityFunctions::print(godot::String("Failed to find action file"));
		}
	}

	if (success) {
		// TODO: Contemplate whether we should parse the JSON ourselves so we know what actions and action sets are available...
		// we can then get action handles for all of them automatically.

		for (std::vector<action_set>::iterator it = action_sets.begin(); it != action_sets.end(); ++it) {
			vr::EVRInputError err = vr::VRInput()->GetActionSetHandle((const char *)it->name.utf8().get_data(), &it->handle);
			if (err != vr::VRInputError_None) {
				Array arr;
				arr.push_back(String(it->name));
				UtilityFunctions::print(String("Failed to obtain action set handle for {0}").format(arr));
			}
		}

		for (auto &input : inputs) {
			// setup handle
			char action_path[1024];
			// TODO at some point support additional action sets
			sprintf(action_path, "%s/in/%s", (const char *)action_sets[0].name.utf8().get_data(), input.path);

			vr::EVRInputError err = vr::VRInput()->GetActionHandle(action_path, &input.handle);
			if (err != vr::VRInputError_None) {
				// maybe output something?
				input.handle = vr::k_ulInvalidActionHandle;

				Array arr;
				arr.push_back(String(action_path));
				UtilityFunctions::print(String("Failed to obtain action handle for {0}").format(arr));
			}
		}

		for (auto &pose : poses) {
			// setup handle
			char action_path[1024];
			// TODO at some point support additional action sets
			sprintf(action_path, "%s/in/%s", (const char *)action_sets[0].name.utf8().get_data(), pose.path);

			vr::EVRInputError err = vr::VRInput()->GetActionHandle(action_path, &pose.handle);
			if (err != vr::VRInputError_None) {
				// maybe output something?
				pose.handle = vr::k_ulInvalidActionHandle;

				Array arr;
				arr.push_back(String(action_path));
				UtilityFunctions::print(String("Failed to obtain action handle for {0}").format(arr));
			}
		}
	}

	if (success) {
		/* create our head tracker */
		head_tracker.instantiate();
		head_tracker->set_tracker_type(XRServer::TRACKER_HEAD);
		head_tracker->set_tracker_name("head");
		head_tracker->set_tracker_desc("HMD tracker");

		XRServer *xr_server = XRServer::get_singleton();
		if (xr_server != nullptr) {
			xr_server->add_tracker(head_tracker);
		}

		/* reset some stuff */
		for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
			tracked_devices[i].tracker = Ref<XRPositionalTracker>();
		}

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
	if (play_area_is_dirty && chaperone != nullptr) {
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

XRPose::TrackingConfidence openvr_data::confidence_from_tracking_result(vr::ETrackingResult p_tracking_result) {
	switch (p_tracking_result) {
		case vr::TrackingResult_Uninitialized:
			return XRPose::XR_TRACKING_CONFIDENCE_NONE;
		case vr::TrackingResult_Running_OK:
			return XRPose::XR_TRACKING_CONFIDENCE_HIGH;
		default:
			return XRPose::XR_TRACKING_CONFIDENCE_LOW;
			break;
	}
}

void openvr_data::process() {
	// we need timing info for one or two things..
	uint64_t msec = Time::get_singleton()->get_ticks_msec();

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
		// Get the predicted game poses for this frame when we called WaitGetPoses right before rendering
		vr::VRCompositor()->GetLastPoses(nullptr, 0, tracked_device_pose, vr::k_unMaxTrackedDeviceCount);
	}

	// update trackers and joysticks
	for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
		// update tracker
		if (i == 0) {
			// TODO make a positional tracker for this too?
			XRPose::TrackingConfidence confidence = XRPose::XR_TRACKING_CONFIDENCE_NONE;
			if (tracked_device_pose[i].bPoseIsValid) {
				confidence = confidence_from_tracking_result(tracked_device_pose[i].eTrackingResult);
				hmd_transform = transform_from_matrix(&tracked_device_pose[i].mDeviceToAbsoluteTracking, 1.0);
				hmd_linear_velocity = Vector3(tracked_device_pose[i].vVelocity.v[0], tracked_device_pose[i].vVelocity.v[1], tracked_device_pose[i].vVelocity.v[2]);
				hmd_angular_velocity = Vector3(tracked_device_pose[i].vAngularVelocity.v[0], tracked_device_pose[i].vAngularVelocity.v[1], tracked_device_pose[i].vAngularVelocity.v[2]);
			}

			if (head_tracker.is_valid()) {
				head_tracker->set_pose("default", hmd_transform, hmd_linear_velocity, hmd_angular_velocity, confidence);
			}

		} else if (tracked_devices[i].tracker.is_valid()) {
			// We'll expose our main transform we got from GetLastPoses as the default pose
			if (tracked_device_pose[i].bPoseIsValid) {
				// update our location and orientation
				XRPose::TrackingConfidence confidence = confidence_from_tracking_result(tracked_device_pose[i].eTrackingResult);
				Transform3D transform = transform_from_matrix(&tracked_device_pose[i].mDeviceToAbsoluteTracking, 1.0);
				Vector3 linear_velocity(tracked_device_pose[i].vVelocity.v[0], tracked_device_pose[i].vVelocity.v[1], tracked_device_pose[i].vVelocity.v[2]);
				Vector3 angular_velocity(tracked_device_pose[i].vAngularVelocity.v[0], tracked_device_pose[i].vAngularVelocity.v[1], tracked_device_pose[i].vAngularVelocity.v[2]);

				tracked_devices[i].tracker->set_pose("default", transform, linear_velocity, angular_velocity, confidence);
			} else {
				tracked_devices[i].tracker->invalidate_pose("default");
			}

			// for our fixed actions we'll hardcode checking our state
			process_device_actions(&tracked_devices[i], msec);
		}
	}

	// TODO add in updating skeleton data
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
	if (hmd != nullptr) {
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

	if (hmd != nullptr) {
		// don't go bigger then this...
		if (pMaxLen > 1024) {
			pMaxLen = 1024;
		}

		if ((hmd != nullptr) && (p_tracked_device_index != vr::k_unTrackedDeviceIndexInvalid)) {
			uint32_t namelength = hmd->GetStringTrackedDeviceProperty(p_tracked_device_index, vr::Prop_RenderModelName_String, nullptr, 0, nullptr);
			if (namelength > 0) {
				if (namelength > pMaxLen) {
					namelength = pMaxLen;
				};

				hmd->GetStringTrackedDeviceProperty(p_tracked_device_index, vr::Prop_RenderModelName_String, returnstring, namelength, nullptr);
			}
		}
	}

	return returnstring;
}

int32_t openvr_data::get_controller_role(vr::TrackedDeviceIndex_t p_tracked_device_index) {
	vr::ETrackedPropertyError error;

	if (hmd == nullptr) {
		return 0;
	}

	int32_t controllerRole = hmd->GetInt32TrackedDeviceProperty(p_tracked_device_index, vr::Prop_ControllerRoleHint_Int32, &error);
	// should add error handling

	return controllerRole;
}

bool openvr_data::is_tracked_device_connected(vr::TrackedDeviceIndex_t p_tracked_device_index) {
	if (hmd == nullptr) {
		return false;
	}

	return hmd->IsTrackedDeviceConnected(p_tracked_device_index);
}

vr::TrackedDeviceClass openvr_data::get_tracked_device_class(vr::TrackedDeviceIndex_t p_tracked_device_index) {
	if (hmd == nullptr) {
		return vr::TrackedDeviceClass_Invalid;
	}

	return hmd->GetTrackedDeviceClass(p_tracked_device_index);
}

Transform3D openvr_data::get_eye_to_head_transform(int p_view, double p_world_scale) {
	if (hmd == nullptr) {
		return Transform3D();
	}

	vr::HmdMatrix34_t matrix = hmd->GetEyeToHeadTransform(p_view == 0 ? vr::Eye_Left : vr::Eye_Right);

	return transform_from_matrix(&matrix, p_world_scale);
}

void openvr_data::pre_render_update() {
	if (get_application_type() != openvr_data::OpenVRApplicationType::OVERLAY) {
		vr::TrackedDevicePose_t tracked_device_pose[vr::k_unMaxTrackedDeviceCount];
		vr::VRCompositor()->WaitGetPoses(tracked_device_pose, vr::k_unMaxTrackedDeviceCount, nullptr, 0);

		// Update our hmd_transform already, we will use this when rendering but it's too late to update it in our node tree
		hmd_transform = transform_from_matrix(&tracked_device_pose[0].mDeviceToAbsoluteTracking, 1.0);
	}
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
		vr::EVRInputError err = vr::VRInput()->GetActionSetHandle((const char *)new_action_set.name.utf8().get_data(), &new_action_set.handle);
		if (err != vr::VRInputError_None) {
			UtilityFunctions::print(String("Failed to obtain action set handle for ") + new_action_set.name);
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

void openvr_data::add_input_action(const char *p_action, const char *p_path, const InputType p_type) {
	for (int i = 0; i < inputs.size(); i++) {
		if (inputs[i].name == p_action) {
			// already registered
			return;
		}
	}

	input_action_info input;
	input.name = p_action;
	input.path = p_path;
	input.type = p_type;
	input.handle = vr::k_ulInvalidActionHandle;
	inputs.push_back(input);
}

void openvr_data::remove_input_action(const char *p_action) {
	for (int i = 0; i < inputs.size(); i++) {
		if (inputs[i].name == p_action) {
			// found it
			inputs.erase(inputs.begin() + i);
		}
	}
}

vr::VRActionHandle_t openvr_data::get_output_action(const char *p_action, const char *p_path) {
	// Outputs are slightly different as we don't register these up front.
	if (!is_initialised()) {
		// called to early
		return vr::k_ulInvalidActionHandle;
	}

	// Find if we have a cached one
	for (int i = 0; i < outputs.size(); i++) {
		if (outputs[i].name == p_action) {
			// found it
			return outputs[i].handle;
		}
	}

	// add a new entry to our cache
	action_info output;
	output.name = p_action;
	output.path = p_path;
	output.handle = vr::k_ulInvalidActionHandle;

	char action_path[1024];
	// TODO at some point support additional action sets
	sprintf(action_path, "%s/out/%s", (const char *)action_sets[0].name.utf8().get_data(), output.path);

	vr::EVRInputError err = vr::VRInput()->GetActionHandle(action_path, &output.handle);
	if (err != vr::VRInputError_None) {
		// maybe output something?
		output.handle = vr::k_ulInvalidActionHandle;

		Array arr;
		arr.push_back(String(action_path));
		UtilityFunctions::print(String("Failed to obtain action handle for {0}").format(arr));
	}

	outputs.push_back(output);
	return output.handle;
}

void openvr_data::trigger_haptic_pulse(const char *p_action, const char *p_device_name, double p_frequency, double p_amplitude, double p_duraction_sec, double p_delay_sec) {
	// we don't have a separate path here..
	vr::VRActionHandle_t action_handle = get_output_action(p_action, p_action);
	if (action_handle == vr::k_ulInvalidActionHandle) {
		// couldn't setup this action...
		return;
	}

	vr::VRInputValueHandle_t source_handle = vr::k_ulInvalidActionHandle;
	if (p_device_name != "") {
		StringName device_name(p_device_name);
		for (int i = 0; i < vr::k_unMaxTrackedDeviceCount && source_handle == vr::k_ulInvalidActionHandle; i++) {
			if (tracked_devices[i].tracker.is_valid() && (tracked_devices[i].tracker->get_tracker_name() == device_name)) {
				source_handle = tracked_devices[i].source_handle;
			}
		}
	}

	vr::EVRInputError err = vr::VRInput()->TriggerHapticVibrationAction(action_handle, p_delay_sec, p_duraction_sec, p_frequency, p_amplitude, source_handle);
	if (err != vr::VRInputError_None) {
		Array arr;
		arr.push_back(String(p_action));
		arr.push_back(String(p_device_name));
		UtilityFunctions::print(String("Failed to trigger haptic pulse {0} for {1}").format(arr));
	}
}

void openvr_data::add_pose_action(const char *p_action, const char *p_path) {
	for (int i = 0; i < poses.size(); i++) {
		if (poses[i].name == p_action) {
			// already registered
			return;
		}
	}

	action_info action;
	action.name = p_action;
	action.path = p_path;
	action.handle = vr::k_ulInvalidActionHandle;
	poses.push_back(action);
}

void openvr_data::remove_pose_action(const char *p_action) {
	for (int i = 0; i < poses.size(); i++) {
		if (poses[i].name == p_action) {
			// found it
			poses.erase(poses.begin() + i);
		}
	}
}

////////////////////////////////////////////////////////////////
// Called when we detect a new device, set it up
void openvr_data::attach_device(uint32_t p_device_index) {
	if (p_device_index == vr::k_unTrackedDeviceIndexInvalid) {
		return;
	}
	tracked_device *device = &tracked_devices[p_device_index];

	if (device->tracker.is_null()) {
		char device_name[256];
		strcpy(device_name, get_device_name(p_device_index, 255));

		vr::TrackedDeviceClass device_class = get_tracked_device_class(p_device_index);
		if (device_class == vr::TrackedDeviceClass_TrackingReference) {
			// ignore base stations and cameras for now
			Array arr;
			arr.push_back(Variant((int64_t)p_device_index));
			arr.push_back(String(device_name));
			UtilityFunctions::print(String("Found base station {0} ({1})").format(arr));
		} else if (device_class == vr::TrackedDeviceClass_HMD) {
			// ignore any HMD
			Array arr;
			arr.push_back(Variant((int64_t)p_device_index));
			arr.push_back(String(device_name));
			UtilityFunctions::print(String("Found HMD {0} ({1})").format(arr));
		} else {
			int64_t hand = 0;

			if (device_class == vr::TrackedDeviceClass_Controller) {
				Array arr;
				arr.push_back(Variant((int64_t)p_device_index));
				arr.push_back(String(device_name));
				UtilityFunctions::print(String("Found controller {0} ({1})").format(arr));

				// If this is a controller than get our controller role
				int32_t controllerRole = get_controller_role(p_device_index);
				if (controllerRole == vr::TrackedControllerRole_RightHand) {
					hand = 2;
				} else if (controllerRole == vr::TrackedControllerRole_LeftHand) {
					hand = 1;
				}
			} else {
				Array arr;
				arr.push_back(Variant((int64_t)p_device_index));
				arr.push_back(String(device_name));
				UtilityFunctions::print(String("Found tracker {0} ({1})").format(arr));
			}

			XRServer *xr_server = XRServer::get_singleton();
			if (xr_server != nullptr) {
				Ref<XRPositionalTracker> new_tracker;
				new_tracker.instantiate();
				new_tracker->set_tracker_type(XRServer::TRACKER_CONTROLLER);
				new_tracker->set_tracker_desc(device_name);
				new_tracker->set_tracker_hand(XRPositionalTracker::TrackerHand(hand));

				if (hand == 1) {
					new_tracker->set_tracker_name("left_hand");
					vr::VRInput()->GetInputSourceHandle("/user/hand/left", &device->source_handle);
				} else if (hand == 2) {
					new_tracker->set_tracker_name("right_hand");
					vr::VRInput()->GetInputSourceHandle("/user/hand/right", &device->source_handle);
				} else {
					// other devices don't have source handles...
					sprintf(device_name, "controller_%i", p_device_index);
					new_tracker->set_tracker_name(device_name);

					device->source_handle = vr::k_ulInvalidInputValueHandle;
				}

				device->tracker = new_tracker;
				xr_server->add_tracker(new_tracker);
			}
		}
	}
}

////////////////////////////////////////////////////////////////
// Called when we lose tracked device, cleanup
void openvr_data::detach_device(uint32_t p_device_index) {
	if (p_device_index == vr::k_unTrackedDeviceIndexInvalid) {
		return;
	}
	tracked_device *device = &tracked_devices[p_device_index];

	if (device->tracker.is_valid()) {
		XRServer *xr_server = XRServer::get_singleton();
		if (xr_server != nullptr) {
			// XXX: Work around a design issue with XRServer: removing a tracker happens by
			// name, instead of removing the exact object you pass. This means that if a
			// tracker has been replaced and then goes inactive, we will remove the wrong one.
			Ref<XRPositionalTracker> existing_tracker = xr_server->get_tracker(
					device->tracker->get_tracker_name());
			if (existing_tracker == device->tracker) {
				xr_server->remove_tracker(device->tracker);
			} else {
				Array arr;
				arr.push_back(device->tracker->get_tracker_name());
				UtilityFunctions::push_warning(String("Not removing tracker {0}, already replaced").format(arr));
			}
		}
		device->tracker.unref();
	}
}

////////////////////////////////////////////////////////////////
// Called by our process loop to handle our fixed actions
void openvr_data::process_device_actions(tracked_device *p_device, uint64_t p_msec) {
	if (p_device->source_handle == vr::k_ulInvalidInputValueHandle) {
		return;
	}

	// Check our action based poses
	for (auto pose : poses) {
		if (pose.handle != vr::k_ulInvalidActionHandle) {
			// TODO change vr::TrackingUniverseStanding to correct value
			vr::InputPoseActionData_t data;
			vr::EVRInputError err = vr::VRInput()->GetPoseActionDataForNextFrame(pose.handle, vr::TrackingUniverseStanding, &data, sizeof(vr::InputPoseActionData_t), p_device->source_handle);
			if (err != vr::VRInputError_None) {
				// No new status
			} else if (!data.bActive) {
				p_device->tracker->invalidate_pose(pose.name);
			} else if (!data.pose.bPoseIsValid) {
				p_device->tracker->invalidate_pose(pose.name);
			} else {
				XRPose::TrackingConfidence confidence = confidence_from_tracking_result(data.pose.eTrackingResult);
				Transform3D transform = transform_from_matrix(&data.pose.mDeviceToAbsoluteTracking, 1.0);
				Vector3 linear_velocity(data.pose.vVelocity.v[0], data.pose.vVelocity.v[1], data.pose.vVelocity.v[2]);
				Vector3 angular_velocity(data.pose.vAngularVelocity.v[0], data.pose.vAngularVelocity.v[1], data.pose.vAngularVelocity.v[2]);

				p_device->tracker->set_pose(pose.name, transform, linear_velocity, angular_velocity, confidence);
			}
		} else {
			p_device->tracker->invalidate_pose(pose.name);
		}
	}

	for (auto input : inputs) {
		if (input.handle != vr::k_ulInvalidActionHandle) {
			switch (input.type) {
				case IT_BOOL: {
					vr::InputDigitalActionData_t action_data;
					vr::EVRInputError err = vr::VRInput()->GetDigitalActionData(input.handle, &action_data, sizeof(action_data), p_device->source_handle);
					if (err == vr::VRInputError_None) {
						bool pressed = action_data.bActive && action_data.bState;
						p_device->tracker->set_input(input.name, pressed);
					}
				} break;
				case IT_FLOAT: {
					vr::InputAnalogActionData_t analog_data;
					vr::EVRInputError err = vr::VRInput()->GetAnalogActionData(input.handle, &analog_data, sizeof(analog_data), p_device->source_handle);
					if (err == vr::VRInputError_None) {
						if (analog_data.bActive) {
							p_device->tracker->set_input(input.name, analog_data.x);
						}
					}
				} break;
				case IT_VECTOR2: {
					vr::InputAnalogActionData_t analog_data;
					vr::EVRInputError err = vr::VRInput()->GetAnalogActionData(input.handle, &analog_data, sizeof(analog_data), p_device->source_handle);
					if (err == vr::VRInputError_None) {
						if (analog_data.bActive) {
							Vector2 value(analog_data.x, analog_data.y);
							p_device->tracker->set_input(input.name, value);
						}
					}
				} break;
				default: break;
			}
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
		UtilityFunctions::print("OpenVR has already been initialised");
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
	if (hmd == nullptr) {
		return 0;
	}

	return render_models->GetRenderModelCount();
}

////////////////////////////////////////////////////////////////
// get the name of a render model at a given index
godot::String openvr_data::get_render_model_name(uint32_t p_model_index) {
	godot::String s;

	if (hmd != nullptr) {
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

	strcpy(new_entry.model_name, p_model_name.utf8().get_data());
	new_entry.mesh = p_mesh;

	load_models.push_back(new_entry);
}

////////////////////////////////////////////////////////////////
// Actually load our model this is called from our process so
// we're in our render thread.
// OpenVR loads this in a separate thread so we repeatedly call this
// until the model is loaded and only then process it
bool openvr_data::_load_render_model(model_mesh *p_model) {
	vr::RenderModel_t *ovr_render_model = nullptr;

	// Load our render model
	vr::EVRRenderModelError err = render_models->LoadRenderModel_Async(p_model->model_name, &ovr_render_model);
	if (err == vr::VRRenderModelError_Loading) {
		// still loading.. check again later
		return false;
	}

	if (err != vr::VRRenderModelError_None) {
		UtilityFunctions::print(String("OpenVR: Couldn''t find model for ") + String(p_model->model_name) + " (" + String::num((int64_t)err) + ")");

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

	/* TODO Fix this
	// load our pool arrays into our array
	arr[ArrayMesh::ARRAY_VERTEX] = vertices;
	arr[ArrayMesh::ARRAY_NORMAL] = normals;
	arr[ArrayMesh::ARRAY_TEX_UV] = texcoords;
	arr[ArrayMesh::ARRAY_INDEX] = indices;

	// and load
	p_model->mesh->add_surface_from_arrays(ArrayMesh::PRIMITIVE_TRIANGLES, arr, blend_array);

	// prepare our material
	Ref<StandardMaterial3D> material;
	material.instantiate();

	// queue loading our textures
	load_texture(TT_ALBEDO, ovr_render_model->diffuseTextureId, material);

	// assign material to our model
	p_model->mesh->surface_set_material(0, material);

	// free up our render model
	render_models->FreeRenderModel(ovr_render_model);
	*/

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
	vr::RenderModel_TextureMap_t *ovr_texture = nullptr;

	// load our texture
	vr::EVRRenderModelError err = vr::VRRenderModels()->LoadTexture_Async(p_texture->texture_id, &ovr_texture);
	if (err == vr::VRRenderModelError_Loading) {
		// still loading.. check again later
		return false;
	}

	if (err != vr::VRRenderModelError_None) {
		UtilityFunctions::print(String("OpenVR: Couldn''t find texture for ") + String::num((int64_t)p_texture->texture_id) + " (" + String::num((int64_t)err) + ")");

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
	image.instantiate();
	image->create_from_data(ovr_texture->unWidth, ovr_texture->unHeight, false, Image::FORMAT_RGBA8, image_data);

	Ref<ImageTexture> texture;
	texture.instantiate();
	texture->create_from_image(image);

	/* TODO fix this!
	switch (p_texture->type) {
		case TT_ALBEDO:
			p_texture->material->set_texture(StandardMaterial3D::TEXTURE_ALBEDO, texture);
			break;
		default: break;
	}
	*/

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
Transform3D openvr_data::transform_from_matrix(vr::HmdMatrix34_t *p_matrix, double p_world_scale) {
	Transform3D ret;

	ret.basis = Basis(
			Vector3(p_matrix->m[0][0], p_matrix->m[1][0], p_matrix->m[2][0]),
			Vector3(p_matrix->m[0][1], p_matrix->m[1][1], p_matrix->m[2][1]),
			Vector3(p_matrix->m[0][2], p_matrix->m[1][2], p_matrix->m[2][2]));

	ret.origin.x = (real_t)(p_matrix->m[0][3] * p_world_scale);
	ret.origin.y = (real_t)(p_matrix->m[1][3] * p_world_scale);
	ret.origin.z = (real_t)(p_matrix->m[2][3] * p_world_scale);

	return ret;
}

////////////////////////////////////////////////////////////////
// Convert a transform from Godot into a matrix OpenVR requires
void openvr_data::matrix_from_transform(vr::HmdMatrix34_t *p_matrix, Transform3D *p_transform, double p_world_scale) {
	p_matrix->m[0][3] = p_transform->origin.x / (real_t)p_world_scale;
	p_matrix->m[1][3] = p_transform->origin.y / (real_t)p_world_scale;
	p_matrix->m[2][3] = p_transform->origin.z / (real_t)p_world_scale;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			p_matrix->m[i][j] = p_transform->basis[j][i];
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
