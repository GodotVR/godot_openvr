#include "openvr_overlay_container.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/rendering_device.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>
#include <godot_cpp/classes/viewport_texture.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "openvr_event_signals.h"

using namespace godot;

void OpenVROverlayContainer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("is_overlay_visible"), &OpenVROverlayContainer::is_overlay_visible);
	ClassDB::bind_method(D_METHOD("set_overlay_visible", "visible"), &OpenVROverlayContainer::set_overlay_visible);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "overlay_visible"), "set_overlay_visible", "is_overlay_visible");

	ClassDB::bind_method(D_METHOD("get_overlay_width_in_meters"), &OpenVROverlayContainer::get_overlay_width_in_meters);
	ClassDB::bind_method(D_METHOD("set_overlay_width_in_meters", "width"), &OpenVROverlayContainer::set_overlay_width_in_meters);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "overlay_width_in_meters"), "set_overlay_width_in_meters", "get_overlay_width_in_meters");

	ClassDB::bind_method(D_METHOD("get_tracked_device_name"), &OpenVROverlayContainer::get_tracked_device_name);
	ClassDB::bind_method(D_METHOD("set_tracked_device_name", "tracked_device"), &OpenVROverlayContainer::set_tracked_device_name);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "tracked_device_name", PROPERTY_HINT_ENUM_SUGGESTION, "hmd,left_hand,right_hand"), "set_tracked_device_name", "get_tracked_device_name");

	ClassDB::bind_method(D_METHOD("get_absolute_position"), &OpenVROverlayContainer::get_absolute_position);
	ClassDB::bind_method(D_METHOD("set_absolute_position", "absolute_position"), &OpenVROverlayContainer::set_absolute_position);
	ADD_PROPERTY(PropertyInfo(Variant::TRANSFORM3D, "absolute_position"), "set_absolute_position", "get_absolute_position");

	ClassDB::bind_method(D_METHOD("get_tracked_device_relative_position"), &OpenVROverlayContainer::get_tracked_device_relative_position);
	ClassDB::bind_method(D_METHOD("set_tracked_device_relative_position", "tracked_device_relative_position"), &OpenVROverlayContainer::set_tracked_device_relative_position);
	ADD_PROPERTY(PropertyInfo(Variant::TRANSFORM3D, "tracked_device_relative_position"), "set_tracked_device_relative_position", "get_tracked_device_relative_position");

	ClassDB::bind_method(D_METHOD("get_flag", "flag"), &OpenVROverlayContainer::get_flag);
	ClassDB::bind_method(D_METHOD("set_flag", "flag", "state"), &OpenVROverlayContainer::set_flag);

	ADD_GROUP("Flags", "");
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "no_dashboard_tab"), "set_flag", "get_flag", vr::VROverlayFlags_NoDashboardTab);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "send_vr_discrete_scroll_events"), "set_flag", "get_flag", vr::VROverlayFlags_SendVRDiscreteScrollEvents);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "send_vr_touchpad_events"), "set_flag", "get_flag", vr::VROverlayFlags_SendVRTouchpadEvents);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "show_touch_pad_scroll_wheel"), "set_flag", "get_flag", vr::VROverlayFlags_ShowTouchPadScrollWheel);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "transfer_ownership_to_internal_process"), "set_flag", "get_flag", vr::VROverlayFlags_TransferOwnershipToInternalProcess);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "side_by_side_parallel"), "set_flag", "get_flag", vr::VROverlayFlags_SideBySide_Parallel);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "side_by_side_crossed"), "set_flag", "get_flag", vr::VROverlayFlags_SideBySide_Crossed);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "panorama"), "set_flag", "get_flag", vr::VROverlayFlags_Panorama);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "stereo_panorama"), "set_flag", "get_flag", vr::VROverlayFlags_StereoPanorama);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "sort_with_non_scene_overlays"), "set_flag", "get_flag", vr::VROverlayFlags_SortWithNonSceneOverlays);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "visible_in_dashboard"), "set_flag", "get_flag", vr::VROverlayFlags_VisibleInDashboard);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "make_overlays_interactive_if_visible"), "set_flag", "get_flag", vr::VROverlayFlags_MakeOverlaysInteractiveIfVisible);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "send_vr_smooth_scroll_events"), "set_flag", "get_flag", vr::VROverlayFlags_SendVRSmoothScrollEvents);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "protected_content"), "set_flag", "get_flag", vr::VROverlayFlags_ProtectedContent);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "hide_laser_interaction"), "set_flag", "get_flag", vr::VROverlayFlags_HideLaserIntersection);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "wants_modal_behavior"), "set_flag", "get_flag", vr::VROverlayFlags_WantsModalBehavior);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "is_premultiplied"), "set_flag", "get_flag", vr::VROverlayFlags_IsPremultiplied);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "ignore_texture_alpha"), "set_flag", "get_flag", vr::VROverlayFlags_IgnoreTextureAlpha);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "enable_control_bar"), "set_flag", "get_flag", vr::VROverlayFlags_EnableControlBar);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "enable_control_bar_keyboard"), "set_flag", "get_flag", vr::VROverlayFlags_EnableControlBarKeyboard);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "enable_control_bar_close"), "set_flag", "get_flag", vr::VROverlayFlags_EnableControlBarClose);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "enable_click_stabilization"), "set_flag", "get_flag", vr::VROverlayFlags_EnableClickStabilization);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "multi_cursor"), "set_flag", "get_flag", vr::VROverlayFlags_MultiCursor);

	ClassDB::bind_method(D_METHOD("on_frame_post_draw"), &OpenVROverlayContainer::on_frame_post_draw);

	// Events
	// See openvr_event_handler.cpp for an explanation of this macro.

	VREVENT_SIGNAL(vr::EVREventType::VREvent_MouseMove, openvr_data::Mouse, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_MouseButtonDown, openvr_data::Mouse, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_MouseButtonUp, openvr_data::Mouse, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_FocusEnter, openvr_data::Overlay, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_FocusLeave, openvr_data::Overlay, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_ScrollDiscrete, openvr_data::Scroll, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_TouchPadMove, openvr_data::Mouse, header);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_ScrollSmooth, openvr_data::Scroll, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_LockMousePosition, openvr_data::Mouse, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_UnlockMousePosition, openvr_data::Mouse, header);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_OverlayShown, openvr_data::None, tested);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_OverlayHidden, openvr_data::None, tested);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_ImageLoaded, openvr_data::Unknown, none);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_OverlayGamepadFocusGained, openvr_data::None, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_OverlayGamepadFocusLost, openvr_data::None, guess);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_ImageFailed, openvr_data::Unknown, none);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_OverlayClosed, openvr_data::Overlay, guess);
}

OpenVROverlayContainer::OpenVROverlayContainer() {
	ovr = openvr_data::retain_singleton();
	overlay_width_in_meters = 1.0;
	overlay_visible = true;
	tracked_device_name = "";
	overlay = 0;
}

OpenVROverlayContainer::~OpenVROverlayContainer() {
	if (ovr != nullptr) {
		ovr->release();
		ovr = nullptr;
	}
}

void OpenVROverlayContainer::_notification(int p_what) {
	if (p_what == NOTIFICATION_READY) {
		if (Engine::get_singleton()->is_editor_hint()) {
			return;
		}

		String appname = ProjectSettings::get_singleton()->get_setting("application/config/name");
		String overlay_identifier = appname + "." + String::num_int64(get_instance_id());

		const char *overlay_key = overlay_identifier.utf8().get_data();
		const char *overlay_name = overlay_key;

		vr::EVROverlayError vrerr = vr::VROverlay()->CreateOverlay(overlay_key, overlay_name, &overlay);
		if (vrerr != vr::VROverlayError_None) {
			Array arr;
			arr.push_back(String::num(vrerr));
			arr.push_back(String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
			UtilityFunctions::print(String("Could not create overlay, OpenVR error: {0}, {1}").format(arr));
		}

		// Tie our new overlay to this container so that events can make it back here later.
		overlay_id = ovr->add_overlay(overlay, this);

		// We have no way of knowing when our SubViewports' textures are actually updated. Connect to the
		// frame_post_draw signal so we can update the overlay every frame just in case.
		RenderingServer::get_singleton()->connect("frame_post_draw", Callable(this, "on_frame_post_draw").bind());

		// TODO: Use the position of this container in a 3d scene, if it has one.
		Transform3D initial_transform;
		initial_transform = initial_transform.translated(Vector3(0, 0, 1) * -1.4);

		set_absolute_position(initial_transform);
		set_overlay_width_in_meters(overlay_width_in_meters);
		set_overlay_visible(overlay_visible);

		for (const vr::VROverlayFlags flag : initial_flags) {
			set_flag(flag, true);
		}
	} else if (p_what == NOTIFICATION_EXIT_TREE) {
		if (!overlay) {
			return;
		}

		vr::EVROverlayError vrerr = vr::VROverlay()->DestroyOverlay(overlay);
		if (vrerr != vr::VROverlayError_None) {
			Array arr;
			arr.push_back(String::num(vrerr));
			arr.push_back(String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
			UtilityFunctions::print(String("Could not destroy overlay, OpenVR error: {0}, {1}").format(arr));
			return;
		}

		ovr->remove_overlay(overlay_id);
		overlay_id = 0;
		overlay = vr::k_ulOverlayHandleInvalid;
	}
}

bool OpenVROverlayContainer::get_flag(vr::VROverlayFlags p_flag) {
	if (overlay == vr::k_ulOverlayHandleInvalid) {
		return initial_flags.has(p_flag);
	}

	bool state;
	vr::EVROverlayError error = vr::VROverlay()->GetOverlayFlag(overlay, p_flag, &state);
	if (error != vr::VROverlayError_None) {
		Array arr;
		arr.push_back(String::num(error));
		arr.push_back(String(vr::VROverlay()->GetOverlayErrorNameFromEnum(error)));
		UtilityFunctions::print(String("Could not get overlay flag, OpenVR error: {0}, {1}").format(arr));
	}

	return state;
}

void OpenVROverlayContainer::set_flag(vr::VROverlayFlags p_flag, bool p_state) {
	if (overlay == vr::k_ulOverlayHandleInvalid) {
		if (p_state) {
			initial_flags.insert(p_flag);
		} else {
			initial_flags.erase(p_flag);
		}

		return;
	}

	vr::EVROverlayError error = vr::VROverlay()->SetOverlayFlag(overlay, p_flag, p_state);
	if (error != vr::VROverlayError_None) {
		Array arr;
		arr.push_back(String::num(error));
		arr.push_back(String(vr::VROverlay()->GetOverlayErrorNameFromEnum(error)));
		UtilityFunctions::print(String("Could not set overlay flag, OpenVR error: {0}, {1}").format(arr));
	}
}

void OpenVROverlayContainer::draw_overlay(const Ref<Texture2D> &p_texture) {
	if (overlay == vr::k_ulOverlayHandleInvalid) {
		return;
	}

	RenderingServer *rendering_server = RenderingServer::get_singleton();
	ERR_FAIL_NULL(rendering_server);
	RenderingDevice *rendering_device = rendering_server->get_rendering_device();
	ERR_FAIL_NULL(rendering_device);

	RID texture_rid = rendering_server->texture_get_rd_texture(p_texture->get_rid());
	uint64_t image = rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_IMAGE, texture_rid, 0);
	uint32_t format = rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_IMAGE_NATIVE_TEXTURE_FORMAT, texture_rid, 0);

	if (image == 0 || format == 0) {
		return;
	}

	Size2 size = get_size();

	vr::VRTextureBounds_t bounds;
	bounds.uMin = 0.0f;
	bounds.uMax = 1.0f;
	bounds.vMin = 0.0f;
	bounds.vMax = 1.0f;

	vr::EVROverlayError vrerr;

	vr::VRVulkanTextureData_t vulkan_data;
	vulkan_data.m_pDevice = (VkDevice_T *)rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_DEVICE, RID(), 0);
	vulkan_data.m_pPhysicalDevice = (VkPhysicalDevice_T *)rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_PHYSICAL_DEVICE, RID(), 0);
	vulkan_data.m_pInstance = (VkInstance_T *)rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_INSTANCE, RID(), 0);
	vulkan_data.m_pQueue = (VkQueue_T *)rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_QUEUE, RID(), 0);
	vulkan_data.m_nQueueFamilyIndex = (uint32_t)rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_VULKAN_QUEUE_FAMILY_INDEX, RID(), 0);

	vulkan_data.m_nImage = image;
	vulkan_data.m_nFormat = format;
	vulkan_data.m_nWidth = size.width;
	vulkan_data.m_nHeight = size.height;
	vulkan_data.m_nSampleCount = 0;

	vr::Texture_t overlay_texture = { &vulkan_data, vr::TextureType_Vulkan, vr::ColorSpace_Gamma };

	vrerr = vr::VROverlay()->SetOverlayTexture(overlay, &overlay_texture);

	if (vrerr != vr::VROverlayError_None) {
		UtilityFunctions::printerr(String("OpenVR could not set texture for overlay: ") + String::num_int64(vrerr) + String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
		return;
	}

	vrerr = vr::VROverlay()->SetOverlayTextureBounds(overlay, &bounds);

	if (vrerr != vr::VROverlayError_None) {
		UtilityFunctions::printerr(String("OpenVR could not set textute bounds for overlay: ") + String::num_int64(vrerr) + String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
		return;
	}
}

void OpenVROverlayContainer::on_frame_post_draw() {
	if (overlay == vr::k_ulOverlayHandleInvalid) {
		return;
	}

	for (int i = 0; i < get_child_count(); i++) {
		SubViewport *c = Object::cast_to<SubViewport>(get_child(i));
		if (!c) {
			continue;
		}
		draw_overlay(c->get_texture());
	}
}

float OpenVROverlayContainer::get_overlay_width_in_meters() {
	if (overlay) {
		vr::VROverlay()->GetOverlayWidthInMeters(overlay, &overlay_width_in_meters);
	}
	return overlay_width_in_meters;
}

void OpenVROverlayContainer::set_overlay_width_in_meters(real_t p_new_size) {
	overlay_width_in_meters = p_new_size;

	if (overlay) {
		vr::EVROverlayError vrerr = vr::VROverlay()->SetOverlayWidthInMeters(overlay, p_new_size);

		if (vrerr != vr::VROverlayError_None) {
			Array arr;
			arr.push_back(String::num(vrerr));
			arr.push_back(String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
			UtilityFunctions::print(String("Could not set overlay width in meters, OpenVR error: {0}, {1}").format(arr));
		}
	}
}

bool OpenVROverlayContainer::is_overlay_visible() {
	if (overlay) {
		overlay_visible = vr::VROverlay()->IsOverlayVisible(overlay);
	}
	return overlay_visible;
}

void OpenVROverlayContainer::set_overlay_visible(bool p_visible) {
	overlay_visible = p_visible;

	if (overlay) {
		if (p_visible) {
			vr::EVROverlayError vrerr = vr::VROverlay()->ShowOverlay(overlay);

			if (vrerr != vr::VROverlayError_None) {
				Array arr;
				arr.push_back(String::num(vrerr));
				arr.push_back(String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
				UtilityFunctions::print(String("Could not show overlay, OpenVR error: {0}, {1}").format(arr));
			}
		} else {
			vr::EVROverlayError vrerr = vr::VROverlay()->HideOverlay(overlay);

			if (vrerr != vr::VROverlayError_None) {
				Array arr;
				arr.push_back(String::num(vrerr));
				arr.push_back(String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
				UtilityFunctions::print(String("Could not hide overlay, OpenVR error: {0}, {1}").format(arr));
			}
		}
	}
}

String OpenVROverlayContainer::get_tracked_device_name() {
	return tracked_device_name;
}

void OpenVROverlayContainer::set_tracked_device_name(String p_tracked_device) {
	tracked_device_name = p_tracked_device;
	update_overlay_transform();
}

Transform3D OpenVROverlayContainer::get_absolute_position() {
	return absolute_position;
}

void OpenVROverlayContainer::set_absolute_position(Transform3D p_position) {
	absolute_position = p_position;
	update_overlay_transform();
}

Transform3D OpenVROverlayContainer::get_tracked_device_relative_position() {
	return tracked_device_relative_position;
}

void OpenVROverlayContainer::set_tracked_device_relative_position(Transform3D p_position) {
	tracked_device_relative_position = p_position;
	update_overlay_transform();
}

bool OpenVROverlayContainer::update_overlay_transform() {
	if (overlay == vr::k_ulOverlayHandleInvalid) {
		return false;
	}

	XRServer *server = XRServer::get_singleton();
	double ws = server->get_world_scale();
	vr::HmdMatrix34_t matrix;
	vr::EVROverlayError vrerr;

	if (tracked_device_name != "") {
		ovr->matrix_from_transform(&matrix, &tracked_device_relative_position, ws);

		vr::TrackedDeviceIndex_t index = vr::k_unTrackedDeviceIndexInvalid;
		if (tracked_device_name == "hmd") {
			index = vr::k_unTrackedDeviceIndex_Hmd;
		} else {
			index = ovr->get_tracked_device_index(server->get_tracker(tracked_device_name));
		}

		if (index == vr::k_unTrackedDeviceIndexInvalid) {
			Array arr;
			arr.push_back(tracked_device_name);
			UtilityFunctions::print(String("Could not track overlay relative to unknown device {0}").format(arr));
			return false;
		}

		vrerr = vr::VROverlay()->SetOverlayTransformTrackedDeviceRelative(overlay, index, &matrix);
	} else {
		vr::TrackingUniverseOrigin origin;

		ovr->matrix_from_transform(&matrix, &absolute_position, ws);

		openvr_data::OpenVRTrackingUniverse tracking_universe = ovr->get_tracking_universe();
		if (tracking_universe == openvr_data::OpenVRTrackingUniverse::SEATED) {
			origin = vr::TrackingUniverseSeated;
		} else if (tracking_universe == openvr_data::OpenVRTrackingUniverse::STANDING) {
			origin = vr::TrackingUniverseStanding;
		} else {
			origin = vr::TrackingUniverseRawAndUncalibrated;
		}

		vrerr = vr::VROverlay()->SetOverlayTransformAbsolute(overlay, origin, &matrix);
	}

	if (vrerr != vr::VROverlayError_None) {
		Array arr;
		arr.push_back(String::num(vrerr));
		arr.push_back(String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
		UtilityFunctions::print(String("Could not set overlay transform, OpenVR error: {0}, {1}").format(arr));

		return false;
	}

	return true;
}
