////////////////////////////////////////////////////////////////////////////////////////////////
// Helper calls and singleton container for accessing openvr

// Note, even though this is pure C code, we're using the C++ compiler as
// Microsoft never updated their C compiler to understand more modern dialects
// and openvr uses pesky things such as namespaces

#include "openvr_data.h"

using namespace godot;

openvr_data *openvr_data::singleton = NULL;

openvr_data::openvr_data() {
	// get some default values
	use_count = 1;
	hmd = NULL;
	render_models = NULL;
	application_type = OpenVRApplicationType::SCENE;
	tracking_universe = OpenVRTrackingUniverse::STANDING;
}

openvr_data::~openvr_data() {
	if (hmd != NULL) {
		cleanup();
	}

	if (singleton == this) {
		singleton = NULL;
	}
}

void openvr_data::cleanup() {
	hmd = NULL;
	render_models = NULL;

	vr::VR_Shutdown();
}

openvr_data *openvr_data::retain_singleton() {
	if (singleton == NULL) {
		singleton = new openvr_data();
	} else {
		singleton->use_count++;
		godot::Godot::print(
				godot::String("Usage count increased to ") + godot::String::num_int64(singleton->use_count));
	}
	return singleton;
}

void openvr_data::release() {
	if (singleton != this) {
		// this should never happen!
		godot::Godot::print("openvr object does not match singleton!");
	} else if (use_count > 1) {
		use_count--;
		godot::Godot::print(godot::String("Usage count decreased to ") + godot::String::num_int64(use_count));
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
	godot::Godot::print("OpenVR: initialising OpenVR context\n");

	bool success = true;
	vr::EVRInitError error = vr::VRInitError_None;

	if (!vr::VR_IsRuntimeInstalled()) {
		godot::Godot::print("SteamVR has not been installed.");
		success = false;
	}

	if (success) {
		// Loading the SteamVR Runtime
		if (application_type == OpenVRApplicationType::OVERLAY) {
			hmd = vr::VR_Init(&error, vr::VRApplication_Overlay);
			godot::Godot::print("Application in overlay mode.");
		} else {
			hmd = vr::VR_Init(&error, vr::VRApplication_Scene);
			godot::Godot::print("Application in scene (normal) mode.");
		}

		if (error != vr::VRInitError_None) {
			success = false;
			godot::Godot::print(godot::String("Unable to init VR runtime: ") + godot::String(vr::VR_GetVRInitErrorAsEnglishDescription(error)));
		} else {
			godot::Godot::print("Main OpenVR interface has been initialized");
		}
	}

	if (success) {
		// render models give us access to mesh representations of the various
		// controllers
		render_models = (vr::IVRRenderModels *)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &error);
		if (!render_models) {
			success = false;

			godot::Godot::print(godot::String("Unable to get render model interface: ") + godot::String(vr::VR_GetVRInitErrorAsEnglishDescription(error)));
		} else {
			godot::Godot::print("Main render models interface has been initialized.");
		}
	}

	if (success) {
		if (!vr::VRCompositor()) {
			success = false;

			godot::Godot::print("Compositor initialization failed. See log file for details.");
		}
	}

	if (success) {
		if (application_type == OpenVRApplicationType::OVERLAY) {
			if (!vr::VROverlay()) {
				success = false;

				godot::Godot::print("Overlay system initialization failed. See log file for details.");
			}
		}
	}

	if (!success) {
		cleanup();
	}

	return success;
}

void openvr_data::process() {
	// check our model loading in reverse
	for (int i = load_models.size() -1; i >= 0; i--) {
		if (_load_render_model(&load_models[i])) {
			load_models.erase(load_models.begin()+i);
		}		
	}

	// check our texture loading in reverse
	for (int i = load_textures.size() -1; i >= 0; i--) {
		if (_load_texture(&load_textures[i])) {
			load_textures.erase(load_textures.begin()+i);
		}
	}
}

vr::VROverlayHandle_t openvr_data::get_overlay() {
	return overlay;
}

void openvr_data::set_overlay(vr::VROverlayHandle_t p_new_value) {
	overlay = p_new_value;
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
char *openvr_data::get_device_name(vr::TrackedDeviceIndex_t p_tracked_device_index, int pMaxLen) {
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

void openvr_data::get_eye_to_head_transform(godot_transform *p_transform, int p_eye, float p_world_scale) {
	if (hmd == NULL) {
		return;
	}

	vr::HmdMatrix34_t matrix = hmd->GetEyeToHeadTransform(p_eye == 1 ? vr::Eye_Left : vr::Eye_Right);

	transform_from_matrix(p_transform, &matrix, p_world_scale);
}

uint32_t openvr_data::get_render_model_count() {
	if (hmd == NULL) {
		return 0;
	}

	return render_models->GetRenderModelCount();
}

godot::String openvr_data::get_render_model_name(uint32_t p_model_index) {
	godot::String s;

	if (hmd != NULL) {
		char model_name[256];
		render_models->GetRenderModelName(p_model_index, model_name, 256);
		s = model_name;
	}

	return s;
}

void openvr_data::load_render_model(const String &p_model_name, ArrayMesh *p_mesh) {
	// if we already have an entry, remove it
	remove_mesh(p_mesh);

	// add an entry, we'll attempt a load 
	model_mesh new_entry;

	CharString name_cs = p_model_name.ascii();
	strcpy(new_entry.model_name, name_cs.get_data());
	new_entry.mesh = p_mesh;

	load_models.push_back(new_entry);
}

bool openvr_data::_load_render_model(model_mesh * p_model) {
	vr::RenderModel_t *ovr_render_model = NULL;

	// Load our render model
	vr::EVRRenderModelError err = render_models->LoadRenderModel_Async(p_model->model_name, &ovr_render_model);
	if (err == vr::VRRenderModelError_Loading) {
		// still loading.. check again later
		return false;
	}

	if (err != vr::VRRenderModelError_None) {
		Godot::print(String("OpenVR: Couldn''t find model for ") + String(p_model->model_name) +" (" + String::num_int64(err) + ")");

		// don't try again, remove it from our list
		return true;
	}

	PoolVector3Array vertices;
	PoolVector3Array normals;
	PoolVector2Array texcoords;
	PoolIntArray indices;
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
		PoolVector3Array::Write vw = vertices.write();
		PoolVector3Array::Write nw = normals.write();
		PoolVector2Array::Write tw = texcoords.write();

		for (int i = 0; i < ovr_render_model->unVertexCount; i++) {
			vw[i] = Vector3(ovr_render_model->rVertexData[i].vPosition.v[0], ovr_render_model->rVertexData[i].vPosition.v[1], ovr_render_model->rVertexData[i].vPosition.v[2]);
			nw[i] = Vector3(ovr_render_model->rVertexData[i].vNormal.v[0], ovr_render_model->rVertexData[i].vNormal.v[1], ovr_render_model->rVertexData[i].vNormal.v[2]);
			tw[i] = Vector2(ovr_render_model->rVertexData[i].rfTextureCoord[0], ovr_render_model->rVertexData[i].rfTextureCoord[1]);
		}
	}


	// copy our indices, for some reason these are other way around :)
	{
		// lock for writing
		PoolIntArray::Write iw = indices.write();

		for (int i = 0; i < ovr_render_model->unTriangleCount * 3; i += 3) {
			iw[i + 0] = ovr_render_model->rIndexData[i+2];
			iw[i + 1] = ovr_render_model->rIndexData[i+1];
			iw[i + 2] = ovr_render_model->rIndexData[i+0];
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
	p_model->mesh->add_surface_from_arrays(ArrayMesh::PRIMITIVE_TRIANGLES, arr, blend_array, ArrayMesh::ARRAY_COMPRESS_DEFAULT);

	// prepare our material
	Ref<SpatialMaterial> material;
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

void openvr_data::load_texture(TextureType p_type, vr::TextureID_t p_texture_id, Ref<SpatialMaterial> p_material) {
	// add an entry, we'll attempt a load 
	texture_material new_entry;

	new_entry.type = p_type;
	new_entry.texture_id = p_texture_id;
	new_entry.material = p_material;

	load_textures.push_back(new_entry);
}


bool openvr_data::_load_texture(texture_material * p_texture) {
	vr::RenderModel_TextureMap_t *ovr_texture = NULL;

	// load our texture
	vr::EVRRenderModelError err = vr::VRRenderModels()->LoadTexture_Async(p_texture->texture_id, &ovr_texture);
	if (err == vr::VRRenderModelError_Loading) {
		// still loading.. check again later
		return false;
	}

	if (err != vr::VRRenderModelError_None) {
		Godot::print(String("OpenVR: Couldn''t find texture for ") + String::num_int64(p_texture->texture_id) +" (" + String::num_int64(err) + ")");

		// reset our references to ensure our material gets freed at the right time
		p_texture->material = Ref<SpatialMaterial>();

		// don't try again, remove it from our list
		return true;
	}

	PoolByteArray image_data;
	image_data.resize(ovr_texture->unWidth * ovr_texture->unHeight * 4);

	{
		PoolByteArray::Write idw = image_data.write();
		memcpy(idw.ptr(), ovr_texture->rubTextureMapData, ovr_texture->unWidth * ovr_texture->unHeight * 4);
	}

	Ref<Image> image;
	image.instance();
	image->create_from_data(ovr_texture->unWidth, ovr_texture->unHeight, false, Image::FORMAT_RGBA8, image_data);

	Ref<ImageTexture> texture;
	texture.instance();
	texture->create_from_image(image, 7);

	switch (p_texture->type) {
		case TT_ALBEDO:
			p_texture->material->set_texture(SpatialMaterial::TEXTURE_ALBEDO, texture);
			break;
		default: break;
	}

	// reset our references to ensure our material gets freed at the right time
	p_texture->material = Ref<SpatialMaterial>();

	// I guess we're done...
	return true;
}


void openvr_data::remove_mesh(ArrayMesh *p_mesh) {
	// check in reverse so we can safely remove things
	for (int i = load_models.size() -1; i >= 0; i--) {
		if (load_models[i].mesh == p_mesh) {
			load_models.erase(load_models.begin()+i);
		}
	}
}

void openvr_data::transform_from_matrix(godot_transform *p_dest, vr::HmdMatrix34_t *p_matrix, godot_real p_world_scale) {
	godot_basis basis;
	godot_vector3 origin;
	float *basis_ptr = (float *)&basis; // Godot can switch between real_t being
	// double or float.. which one is used...

	int k = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			basis_ptr[k++] = p_matrix->m[i][j];
		}
	}

	godot::api->godot_vector3_new(&origin, p_matrix->m[0][3] * p_world_scale, p_matrix->m[1][3] * p_world_scale, p_matrix->m[2][3] * p_world_scale);
	godot::api->godot_transform_new(p_dest, &basis, &origin);
}

void openvr_data::matrix_from_transform(vr::HmdMatrix34_t *p_matrix, godot_transform *p_transform, godot_real p_world_scale) {
	godot::Transform *transform = (godot::Transform *)p_transform;

	p_matrix->m[0][3] = transform->origin.x / p_world_scale;
	p_matrix->m[1][3] = transform->origin.y / p_world_scale;
	p_matrix->m[2][3] = transform->origin.z / p_world_scale;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			p_matrix->m[i][j] = transform->basis[i][j];
		}
	}
}
