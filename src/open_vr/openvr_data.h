////////////////////////////////////////////////////////////////////////////////////////////////
// Helper calls and singleton container for accessing openvr

#ifndef OPENVR_DATA_H
#define OPENVR_DATA_H

#include <openvr.h>
#include <Godot.hpp>
#include <String.hpp>
#include <Ref.hpp>
#include <ArrayMesh.hpp>
#include <Image.hpp>
#include <ImageTexture.hpp>
#include <SpatialMaterial.hpp>

#include <vector>

class openvr_data {
public:
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

private:
	static openvr_data *singleton;

	int use_count;

	vr::IVRRenderModels *render_models;

	// structure to record which overlays go with which viewport
	struct overlay {
		vr::VROverlayHandle_t handle;
		godot::RID viewport_rid;
	};

	std::vector<overlay> overlays;

	OpenVRApplicationType application_type;
	OpenVRTrackingUniverse tracking_universe;

	// structure to record which model we're loading for our mesh so we can async load this.
	struct model_mesh {
		char model_name[1024];
		godot::ArrayMesh *mesh;
	};

	std::vector<model_mesh> load_models;

	bool _load_render_model(model_mesh * p_model);

	// structure to record which texture we're loading for our mesh so we can async load this.
	enum TextureType {
		TT_ALBEDO,
		TT_MAX
	};

	struct texture_material {
		TextureType type;
		vr::TextureID_t texture_id;
		godot::Ref<godot::SpatialMaterial> material;
	};

	std::vector<texture_material> load_textures;

	void load_texture(TextureType p_type, vr::TextureID_t p_texture_id, godot::Ref<godot::SpatialMaterial> p_material);
	bool _load_texture(texture_material * p_texture);

public:
	vr::IVRSystem *hmd; // make this private?

	openvr_data();
	~openvr_data();
	void cleanup();

	static openvr_data *retain_singleton();
	void release();

	bool is_initialised();
	bool initialise();
	void process();

	// properties
	int get_overlay_count();
	overlay get_overlay(int p_overlay_id);
	int add_overlay(vr::VROverlayHandle_t p_new_value, godot::RID p_viewport_rid);
	void remove_overlay(int p_overlay_id);
	OpenVRApplicationType get_application_type();
	void set_application_type(OpenVRApplicationType p_new_value);
	OpenVRTrackingUniverse get_tracking_universe();
	void set_tracking_universe(OpenVRTrackingUniverse p_new_value);

	// interact with openvr
	void get_recommended_rendertarget_size(uint32_t *p_width, uint32_t *p_height);
	char *get_device_name(vr::TrackedDeviceIndex_t p_tracked_device_index, int pMaxLen);
	int32_t get_controller_role(vr::TrackedDeviceIndex_t p_tracked_device_index);
	bool is_tracked_device_connected(vr::TrackedDeviceIndex_t p_tracked_device_index);
	vr::TrackedDeviceClass get_tracked_device_class(vr::TrackedDeviceIndex_t p_tracked_device_index);

	void get_eye_to_head_transform(godot_transform *p_transform, int p_eye, float p_world_scale = 1.0);

	// interact with render models
	uint32_t get_render_model_count();
	godot::String get_render_model_name(uint32_t p_model_index);
	void load_render_model(const godot::String &p_model_name, godot::ArrayMesh *p_mesh);

	// clear async data
	void remove_mesh(godot::ArrayMesh *p_mesh);

	// helper functions
	void transform_from_matrix(godot_transform *p_dest, vr::HmdMatrix34_t *p_matrix, godot_real p_world_scale);
	void matrix_from_transform(vr::HmdMatrix34_t *p_matrix, godot_transform *p_transform, godot_real p_world_scale);
};

#endif /* !OPENVR_DATA_H */
