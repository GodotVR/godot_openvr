////////////////////////////////////////////////////////////////////////////////////////////////
// Helper calls and singleton container for accessing openvr

#ifndef OPENVR_DATA_H
#define OPENVR_DATA_H

#ifdef USE_OPENVR_MINGW_HEADER
// When compiling with mingw, we need to use the tunabrain-patched header file
// which is packaged by msys2 with the following name.
#include <openvr_mingw.hpp>
#else
#include <openvr.h>
#endif

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/xr_positional_tracker.hpp>
#include <godot_cpp/classes/xr_server.hpp>
#include <godot_cpp/godot.hpp>

#include <vector>

namespace godot {
class openvr_data {
public:
	// enums
	enum OpenVRApplicationType {
		OTHER,
		SCENE,
		OVERLAY
	};

	enum OpenVRTrackingUniverse {
		SEATED,
		STANDING,
		RAW
	};

	struct overlay {
		vr::VROverlayHandle_t handle;
		ObjectID container_instance_id;
	};

private:
	static openvr_data *singleton;

	int use_count;

	vr::IVRRenderModels *render_models;

	std::vector<overlay> overlays;

	OpenVRApplicationType application_type;
	OpenVRTrackingUniverse tracking_universe;

	vr::IVRChaperone *chaperone;
	bool play_area_is_dirty;
	godot::Vector3 play_area[4];
	void update_play_area();

	// interact with openvr
	char *get_device_name(vr::TrackedDeviceIndex_t p_tracked_device_index, uint32_t pMaxLen);
	int32_t get_controller_role(vr::TrackedDeviceIndex_t p_tracked_device_index);
	bool is_tracked_device_connected(vr::TrackedDeviceIndex_t p_tracked_device_index);
	vr::TrackedDeviceClass get_tracked_device_class(vr::TrackedDeviceIndex_t p_tracked_device_index);

	////////////////////////////////////////////////////////////////
	// actions

	struct action_set {
		vr::VRActionSetHandle_t handle;
		godot::String name;
		bool is_active;
	};

	std::vector<action_set> action_sets;

	std::vector<vr::VRActiveActionSet_t> active_action_sets;
	int active_action_set_count = 0;

	enum InputType {
		IT_BOOL,
		IT_FLOAT,
		IT_VECTOR2
	};

	struct input_action_info {
		godot::String name;
		const char *path;
		InputType type;
		vr::VRActionHandle_t handle;
	};

	std::vector<input_action_info> inputs;

	struct action_info {
		godot::String name;
		const char *path;
		vr::VRActionHandle_t handle;
	};

	std::vector<action_info> outputs;
	std::vector<action_info> poses;

	////////////////////////////////////////////////////////////////
	// tracked devices
	Ref<XRPositionalTracker> head_tracker;
	godot::Transform3D hmd_transform;
	godot::Vector3 hmd_linear_velocity;
	godot::Vector3 hmd_angular_velocity;

	struct tracked_device {
		Ref<XRPositionalTracker> tracker;
		vr::VRInputValueHandle_t source_handle;
	};

	bool device_hands_are_available;
	uint32_t left_hand_device;
	uint32_t right_hand_device;

	tracked_device tracked_devices[vr::k_unMaxTrackedDeviceCount];

	void attach_device(uint32_t p_device_index);
	void detach_device(uint32_t p_device_index);
	void process_device_actions(tracked_device *p_device, uint64_t p_msec);
	XRPose::TrackingConfidence confidence_from_tracking_result(vr::ETrackingResult p_tracking_result);

	////////////////////////////////////////////////////////////////
	// meshes (should see about moving this into a separate class)

	// structure to record which model we're loading for our mesh so we can async load this.
	struct model_mesh {
		char model_name[1024];
		godot::ArrayMesh *mesh;
	};

	std::vector<model_mesh> load_models;

	bool _load_render_model(model_mesh *p_model);

	// structure to record which texture we're loading for our mesh so we can async load this.
	enum TextureType {
		TT_ALBEDO,
		TT_MAX
	};

	struct texture_material {
		TextureType type;
		vr::TextureID_t texture_id;
		godot::Ref<godot::StandardMaterial3D> material;
	};

	std::vector<texture_material> load_textures;

	void load_texture(TextureType p_type, vr::TextureID_t p_texture_id, godot::Ref<godot::StandardMaterial3D> p_material);
	bool _load_texture(texture_material *p_texture);

public:
	vr::IVRSystem *hmd; // make this private?

	openvr_data();
	~openvr_data();
	void cleanup();

	static openvr_data *retain_singleton();
	void release();

	////////////////////////////////////////////////////////////////
	// interact with openvr

	bool is_initialised();
	bool initialise();
	void process();

	void get_recommended_rendertarget_size(uint32_t *p_width, uint32_t *p_height);
	godot::Transform3D get_eye_to_head_transform(int p_eye, double p_world_scale = 1.0);

	void pre_render_update();

	// interact with tracking info
	const godot::Transform3D get_hmd_transform() const;
	vr::TrackedDeviceIndex_t get_tracked_device_index(Ref<XRPositionalTracker> p_tracker);

	////////////////////////////////////////////////////////////////
	// overlay
	int get_overlay_count();
	overlay *get_overlay(int p_overlay_id);
	int add_overlay(vr::VROverlayHandle_t p_new_value, ObjectID p_container_instance_id);
	void remove_overlay(int p_overlay_id);

	////////////////////////////////////////////////////////////////
	// setup

	OpenVRApplicationType get_application_type();
	void set_application_type(OpenVRApplicationType p_new_value);
	OpenVRTrackingUniverse get_tracking_universe();
	void set_tracking_universe(OpenVRTrackingUniverse p_new_value);

	bool play_area_available() const;
	const godot::Vector3 *get_play_area() const;

	////////////////////////////////////////////////////////////////
	// action set

	bool set_action_manifest_path(const godot::String p_path);
	godot::String get_default_action_set() const;
	void set_default_action_set(const godot::String p_name);
	int register_action_set(const godot::String p_action_set);
	void set_active_action_set(const godot::String p_action_set);
	void toggle_action_set_active(const godot::String p_action_set, bool p_is_active);
	bool is_action_set_active(const godot::String p_action_set) const;

	void add_input_action(const char *p_action, const char *p_path, const InputType p_type);
	void remove_input_action(const char *p_action);

	vr::VRActionHandle_t get_output_action(const char *p_action, const char *p_path);
	void trigger_haptic_pulse(const char *p_action, const char *p_device_name, double p_frequency, double p_amplitude, double p_duraction_sec, double p_delay_sec);

	void add_pose_action(const char *p_action, const char *p_path);
	void remove_pose_action(const char *p_action);

	////////////////////////////////////////////////////////////////
	// interact with render models
	uint32_t get_render_model_count();
	godot::String get_render_model_name(uint32_t p_model_index);
	void load_render_model(const godot::String &p_model_name, godot::ArrayMesh *p_mesh);

	// clear async data
	void remove_mesh(godot::ArrayMesh *p_mesh);

	////////////////////////////////////////////////////////////////
	// helper functions
	Transform3D transform_from_matrix(vr::HmdMatrix34_t *p_matrix, double p_world_scale);
	void matrix_from_transform(vr::HmdMatrix34_t *p_matrix, Transform3D *p_transform, double p_world_scale);
	void transform_from_bone(Transform3D &p_transform, const vr::VRBoneTransform_t *p_bone_transform);
};
} // namespace godot

#endif /* !OPENVR_DATA_H */
