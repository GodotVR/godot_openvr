////////////////////////////////////////////////////////////////////////////////////////////////
// Our main XRInterface code for our OpenVR GDExtension module

#include "xr_interface_openvr.h"

#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/rendering_device.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void XRInterfaceOpenVR::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_application_type"), &XRInterfaceOpenVR::get_application_type);
	ClassDB::bind_method(D_METHOD("set_application_type", "application_type"), &XRInterfaceOpenVR::set_application_type);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "application_type", PROPERTY_HINT_ENUM, "Other,Scene,Overlay"), "set_application_type", "get_application_type");

	ClassDB::bind_method(D_METHOD("get_tracking_universe"), &XRInterfaceOpenVR::get_tracking_universe);
	ClassDB::bind_method(D_METHOD("set_tracking_universe", "tracking_universe"), &XRInterfaceOpenVR::set_tracking_universe);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "tracking_universe", PROPERTY_HINT_ENUM, "Seated,Standing,Raw"), "set_tracking_universe", "get_tracking_universe");

	ClassDB::bind_method(D_METHOD("get_default_action_set"), &XRInterfaceOpenVR::get_default_action_set);
	ClassDB::bind_method(D_METHOD("set_default_action_set"), &XRInterfaceOpenVR::set_default_action_set);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "default_action_set"), "set_default_action_set", "get_default_action_set");

	ClassDB::bind_method(D_METHOD("register_action_set"), &XRInterfaceOpenVR::register_action_set);
	ClassDB::bind_method(D_METHOD("set_active_action_set"), &XRInterfaceOpenVR::set_active_action_set);
	ClassDB::bind_method(D_METHOD("toggle_action_set_active"), &XRInterfaceOpenVR::toggle_action_set_active);
	ClassDB::bind_method(D_METHOD("is_action_set_active"), &XRInterfaceOpenVR::is_action_set_active);

	ClassDB::bind_method(D_METHOD("play_area_available"), &XRInterfaceOpenVR::play_area_available);
	ClassDB::bind_method(D_METHOD("get_play_area"), &XRInterfaceOpenVR::get_play_area);

	ClassDB::bind_method(D_METHOD("get_device_battery_percentage"), &XRInterfaceOpenVR::get_device_battery_percentage);
	ClassDB::bind_method(D_METHOD("is_device_charging"), &XRInterfaceOpenVR::is_device_charging);
}

int XRInterfaceOpenVR::get_application_type() const {
	if (ovr == nullptr) {
		return 0;
	}

	return ovr->get_application_type();
}

void XRInterfaceOpenVR::set_application_type(int p_type) {
	if (ovr != nullptr) {
		ovr->set_application_type((openvr_data::OpenVRApplicationType)p_type);
	}
}

int XRInterfaceOpenVR::get_tracking_universe() const {
	if (ovr == nullptr) {
		return 0;
	}
	return ovr->get_tracking_universe();
}

void XRInterfaceOpenVR::set_tracking_universe(int p_universe) {
	if (ovr != nullptr) {
		ovr->set_tracking_universe((openvr_data::OpenVRTrackingUniverse)p_universe);
	}
}

String XRInterfaceOpenVR::get_default_action_set() const {
	if (ovr == nullptr) {
		return String();
	}

	return ovr->get_default_action_set();
}

void XRInterfaceOpenVR::set_default_action_set(const String p_name) {
	if (ovr != nullptr) {
		ovr->set_default_action_set(p_name);
	}
}

void XRInterfaceOpenVR::register_action_set(const String p_action_set) {
	if (ovr != nullptr) {
		ovr->register_action_set(p_action_set);
	}
}

void XRInterfaceOpenVR::set_active_action_set(const String p_action_set) {
	if (ovr != nullptr) {
		ovr->set_active_action_set(p_action_set);
	}
}

void XRInterfaceOpenVR::toggle_action_set_active(const String p_action_set, const bool p_is_active) {
	if (ovr != nullptr) {
		ovr->toggle_action_set_active(p_action_set, p_is_active);
	}
}

bool XRInterfaceOpenVR::is_action_set_active(const String p_action_set) const {
	if (ovr == nullptr) {
		return false;
	}

	return ovr->is_action_set_active(p_action_set);
}

bool XRInterfaceOpenVR::play_area_available() const {
	if (ovr == nullptr) {
		return false;
	}

	return ovr->play_area_available();
}

PackedVector3Array XRInterfaceOpenVR::get_play_area() const {
	if (ovr == nullptr || xr_server == nullptr) {
		return PackedVector3Array();
	}

	const Vector3 *play_area = ovr->get_play_area();
	Transform3D reference = xr_server->get_reference_frame();
	double ws = xr_server->get_world_scale();

	PackedVector3Array arr;
	arr.resize(4);

	for (int i = 0; i < 4; i++) {
		arr[i] = reference.xform_inv(play_area[i]) * (real_t)ws;
	}

	return arr;
}

float XRInterfaceOpenVR::get_device_battery_percentage(vr::TrackedDeviceIndex_t p_tracked_device_index) {
	if (ovr == nullptr) {
		return 0.0;
	}

	vr::ETrackedPropertyError pError;
	float battery_percentage = ovr->hmd->GetFloatTrackedDeviceProperty(p_tracked_device_index, vr::Prop_DeviceBatteryPercentage_Float, &pError);

	if (pError != vr::TrackedProp_Success) {
		Array arr;
		arr.push_back(Variant(pError));
		arr.push_back(String(ovr->hmd->GetPropErrorNameFromEnum(pError)));
		UtilityFunctions::print(String("Could not get battery percentage, OpenVR error: {0}, {1} ").format(arr));
	}

	return battery_percentage;
}

bool XRInterfaceOpenVR::is_device_charging(vr::TrackedDeviceIndex_t p_tracked_device_index) {
	if (ovr == nullptr) {
		return false;
	}

	vr::ETrackedPropertyError pError;
	bool is_charging = ovr->hmd->GetBoolTrackedDeviceProperty(p_tracked_device_index, vr::Prop_DeviceIsCharging_Bool, &pError);

	if (pError != vr::TrackedProp_Success) {
		Array arr;
		arr.push_back(Variant(pError));
		arr.push_back(String(ovr->hmd->GetPropErrorNameFromEnum(pError)));
		UtilityFunctions::print(String("Could not get charging state, OpenVR error: {0}, {1} ").format(arr));
	}

	return is_charging;
}

////////////////////////////////////////////////////////////////
// Returns the name of this interface
StringName XRInterfaceOpenVR::_get_name() const {
	StringName name("OpenVR");
	return name;
}

////////////////////////////////////////////////////////////////
// Returns capabilities for this interface
uint32_t XRInterfaceOpenVR::_get_capabilities() const {
	return XR_STEREO + XR_EXTERNAL;
}

////////////////////////////////////////////////////////////////
// Returns whether our interface was successfully initialised
bool XRInterfaceOpenVR::_is_initialized() const {
	if (ovr == nullptr || xr_server == nullptr) {
		return false;
	}

	return ovr->is_initialised();
}

////////////////////////////////////////////////////////////////
// Initialise our interface, sets up OpenVR and starts sending
// output to our HMD
bool XRInterfaceOpenVR::_initialize() {
	if (ovr == nullptr || xr_server == nullptr) {
		return false;
	}

	if (ovr->initialise()) {
		// go and get our recommended target size
		ovr->get_recommended_rendertarget_size(&width, &height);

		xr_server->set_primary_interface(this);
	}

	// and return our result
	return ovr->is_initialised();
}

////////////////////////////////////////////////////////////////
// Uninitialises our interface, shuts down our HMD
void XRInterfaceOpenVR::_uninitialize() {
	if (ovr == nullptr || xr_server == nullptr) {
		return;
	}

	ovr->cleanup();

	// FIX ME - This crashes when this is called on final cleanup...
	// if (xr_server->get_primary_interface() == this) {
	//	xr_server->set_primary_interface(Ref<XRInterface>());
	//}
}

////////////////////////////////////////////////////////////////
// Returns our current tracking status
XRInterface::TrackingStatus XRInterfaceOpenVR::_get_tracking_status() const {
	// TODO implement this..
	return XRInterface::XR_UNKNOWN_TRACKING;
}

////////////////////////////////////////////////////////////////
// Issue a haptic pulse
void XRInterfaceOpenVR::_trigger_haptic_pulse(const String &action_name, const StringName &tracker_name, double frequency, double amplitude, double duration_sec, double delay_sec) {
	if (ovr != nullptr && ovr->is_initialised()) {
		String tname = tracker_name;
		ovr->trigger_haptic_pulse(action_name.utf8().get_data(), tname.utf8().get_data(), frequency, amplitude, duration_sec, delay_sec);
	}
}

////////////////////////////////////////////////////////////////
// Returns the requested size of our render target
// called right before rendering, if the size changes a new
// render target will be constructed.
Vector2 XRInterfaceOpenVR::_get_render_target_size() {
	Vector2 size;

	if (ovr != nullptr && ovr->is_initialised()) {
		// TODO: we should periodically check if the recommended size has changed (the user can adjust this) and if so update our width/height
		// and reset our render texture (RID)

		size.x = width;
		size.y = height;
	}

	return size;
}

////////////////////////////////////////////////////////////////
// Informs Godot how many views are required
uint32_t XRInterfaceOpenVR::_get_view_count() {
	return 2;
}

////////////////////////////////////////////////////////////////
// This is called to get our center transform
Transform3D XRInterfaceOpenVR::_get_camera_transform() {
	if (ovr == nullptr || xr_server == nullptr) {
		return Transform3D();
	}

	Transform3D hmd_transform = ovr->get_hmd_transform();
	hmd_transform.origin *= xr_server->get_world_scale();

	return xr_server->get_reference_frame() * hmd_transform;
}

////////////////////////////////////////////////////////////////
// This is called while rendering to get each view matrix
Transform3D XRInterfaceOpenVR::_get_transform_for_view(uint32_t p_view, const Transform3D &p_cam_transform) {
	if (ovr == nullptr || xr_server == nullptr) {
		return Transform3D();
	}

	// TODO this needs to get a proper entry point, we're just cheating here...
	if (p_view == 0) {
		ovr->pre_render_update();
	}

	double world_scale = xr_server->get_world_scale();

	Transform3D transform_for_view = ovr->get_eye_to_head_transform(p_view, world_scale);

	Transform3D hmd_transform = ovr->get_hmd_transform();
	hmd_transform.origin *= world_scale;

	return p_cam_transform * xr_server->get_reference_frame() * hmd_transform * transform_for_view;
}

////////////////////////////////////////////////////////////////
// This is called while rendering to get each eyes projection matrix
PackedFloat64Array XRInterfaceOpenVR::_get_projection_for_view(uint32_t p_view, double p_aspect, double p_z_near, double p_z_far) {
	PackedFloat64Array arr;

	if (ovr == nullptr || xr_server == nullptr) {
		return arr;
	}

	if (ovr->is_initialised()) {
		arr.resize(16);

		vr::HmdMatrix44_t matrix = ovr->hmd->GetProjectionMatrix(p_view == 0 ? vr::Eye_Left : vr::Eye_Right, (float)p_z_near, (float)p_z_far);

		int k = 0;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				arr[k++] = matrix.m[j][i];
			}
		}
	}

	return arr;
}

////////////////////////////////////////////////////////////////
// This is called after we render a frame so we can send the render output to OpenVR
void XRInterfaceOpenVR::_post_draw_viewport(const RID &p_render_target, const Rect2 &p_screen_rect) {
	// Note that at this point in time nothing has actually been rendered yet, this entry point gets called by Godot after
	// all the rendering for our viewport has been prepared, but the queues have yet to be submitted to Vulkan

	if (p_screen_rect.has_area()) {
		// just blit left eye out to screen
		Rect2 src_rect;
		Rect2 dst_rect = p_screen_rect;
		if (dst_rect.size.x > 0.0 && dst_rect.size.y > 0.0) {
			float src_height = width * (dst_rect.size.y / dst_rect.size.x); // height of our screen mapped to source space
			if (src_height < height) {
				src_height /= height;
				src_rect.position = Vector2(0.0, 0.5 * (1.0 - src_height));
				src_rect.size = Vector2(1.0, src_height);
			} else {
				float src_width = height * (dst_rect.get_size().x / dst_rect.get_size().y); // width of our screen mapped to source space
				src_width /= width;
				src_rect.position = Vector2(0.5 * (1.0 - src_width), 0.0);
				src_rect.size = Vector2(src_width, 1.0);
			}

			add_blit(p_render_target, src_rect, dst_rect, true, 0, false, Vector2(), 0.0, 0.0, 0.0, 0.0);
		}
	}

	texture_rid = get_render_target_texture(p_render_target);
};

void XRInterfaceOpenVR::_end_frame() {
	// _end_frame gets called after Godot has fully prepared its rendering pipeline and submitted its rendering queues to Vulkan
	if (!ovr->is_initialised()) {
		return;
	}

	if (!texture_rid.is_valid()) {
		// No texture means we're not actually rendering a full scene. Probably in overlay mode, but regardless we bail out of this.
		return;
	}

	RenderingServer *rendering_server = RenderingServer::get_singleton();
	ERR_FAIL_NULL(rendering_server);
	RenderingDevice *rendering_device = rendering_server->get_rendering_device();
	ERR_FAIL_NULL(rendering_device);

	uint64_t image = rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_IMAGE, texture_rid, 0);
	uint32_t format = rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_IMAGE_NATIVE_TEXTURE_FORMAT, texture_rid, 0);

	// and now sent to OpenVR...
	if (image != 0 && format != 0) {
		// Submit to SteamVR
		vr::VRTextureBounds_t bounds;
		bounds.uMin = 0.0f;
		bounds.uMax = 1.0f;
		bounds.vMin = 0.0f;
		bounds.vMax = 1.0f;

		vr::VRVulkanTextureArrayData_t vulkan_data_left;
		vulkan_data_left.m_pDevice = (VkDevice_T *)rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_DEVICE, RID(), 0);
		vulkan_data_left.m_pPhysicalDevice = (VkPhysicalDevice_T *)rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_PHYSICAL_DEVICE, RID(), 0);
		vulkan_data_left.m_pInstance = (VkInstance_T *)rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_INSTANCE, RID(), 0);
		vulkan_data_left.m_pQueue = (VkQueue_T *)rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_QUEUE, RID(), 0);
		vulkan_data_left.m_nQueueFamilyIndex = (uint32_t)rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_QUEUE_FAMILY_INDEX, RID(), 0);

		vulkan_data_left.m_nImage = image;
		vulkan_data_left.m_nFormat = format;
		vulkan_data_left.m_nWidth = width;
		vulkan_data_left.m_nHeight = height;
		vulkan_data_left.m_nSampleCount = 0;
		vulkan_data_left.m_unArraySize = 2;
		vulkan_data_left.m_unArrayIndex = 0;

		vr::Texture_t texture_left = { &vulkan_data_left, vr::TextureType_Vulkan, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Left, &texture_left, &bounds, vr::Submit_VulkanTextureWithArrayData);

		vr::VRVulkanTextureArrayData_t vulkan_data_right;
		vulkan_data_right.m_pDevice = (VkDevice_T *)rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_DEVICE, RID(), 0);
		vulkan_data_right.m_pPhysicalDevice = (VkPhysicalDevice_T *)rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_PHYSICAL_DEVICE, RID(), 0);
		vulkan_data_right.m_pInstance = (VkInstance_T *)rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_INSTANCE, RID(), 0);
		vulkan_data_right.m_pQueue = (VkQueue_T *)rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_QUEUE, RID(), 0);
		vulkan_data_right.m_nQueueFamilyIndex = (uint32_t)rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_QUEUE_FAMILY_INDEX, RID(), 0);

		vulkan_data_right.m_nImage = image;
		vulkan_data_right.m_nFormat = format;
		vulkan_data_right.m_nWidth = width;
		vulkan_data_right.m_nHeight = height;
		vulkan_data_right.m_nSampleCount = 0;
		vulkan_data_right.m_unArraySize = 2;
		vulkan_data_right.m_unArrayIndex = 1;

		vr::Texture_t texture_right = { &vulkan_data_right, vr::TextureType_Vulkan, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Right, &texture_right, &bounds, vr::Submit_VulkanTextureWithArrayData);
	}
}

////////////////////////////////////////////////////////////////
// Process is called by the rendering thread right before we
// render our next frame. Here we obtain our new poses.
// The HMD pose is used right away but tracker poses are used
// next frame.
void XRInterfaceOpenVR::_process() {
	if (ovr != nullptr && ovr->is_initialised()) {
		// Call process on our ovr system.
		ovr->process();
	}
}

XRInterfaceOpenVR::XRInterfaceOpenVR() {
	ovr = openvr_data::retain_singleton();
	if (ovr == nullptr) {
		UtilityFunctions::printerr("Couldn't obtain OpenVR singleton");
	}

	xr_server = XRServer::get_singleton();
	if (xr_server == nullptr) {
		UtilityFunctions::printerr("Couldn't obtain XRServer singleton");
	}
}

XRInterfaceOpenVR::~XRInterfaceOpenVR() {
	if (ovr != nullptr) {
		// this should have already been called... But just in case...
		_uninitialize();

		ovr->release();
		ovr = nullptr;
	}

	xr_server = nullptr;
}
