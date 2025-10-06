#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC optimize("no-var-tracking") // GCC's variable assignment tracking chokes on the huge number of structs in _bind_methods.

#include "openvr_event_handler.h"
#include "openvr_event_signals.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void OpenVREventHandler::_bind_methods() {
	ClassDB::bind_method(D_METHOD("register_event_signal", "event_id", "type", "signal_name"), &OpenVREventHandler::register_event_signal);

	// Expose the event type constants for use in GDScript land with `register_event_signal`.
	BIND_ENUM_CONSTANT(openvr_data::None);
	BIND_ENUM_CONSTANT(openvr_data::Controller);
	BIND_ENUM_CONSTANT(openvr_data::Mouse);
	BIND_ENUM_CONSTANT(openvr_data::Scroll);
	BIND_ENUM_CONSTANT(openvr_data::Process);
	BIND_ENUM_CONSTANT(openvr_data::Notification);
	BIND_ENUM_CONSTANT(openvr_data::Overlay);
	BIND_ENUM_CONSTANT(openvr_data::Status);
	BIND_ENUM_CONSTANT(openvr_data::Keyboard);
	BIND_ENUM_CONSTANT(openvr_data::Ipd);
	BIND_ENUM_CONSTANT(openvr_data::Chaperone);
	BIND_ENUM_CONSTANT(openvr_data::PerformanceTest);
	BIND_ENUM_CONSTANT(openvr_data::TouchPadMove);
	BIND_ENUM_CONSTANT(openvr_data::SeatedZeroPoseReset);
	BIND_ENUM_CONSTANT(openvr_data::Screenshot);
	BIND_ENUM_CONSTANT(openvr_data::ScreenshotProgress);
	BIND_ENUM_CONSTANT(openvr_data::ApplicationLaunch);
	BIND_ENUM_CONSTANT(openvr_data::EditingCameraSurface);
	BIND_ENUM_CONSTANT(openvr_data::MessageOverlay);
	BIND_ENUM_CONSTANT(openvr_data::Property);
	BIND_ENUM_CONSTANT(openvr_data::HapticVibration);
	BIND_ENUM_CONSTANT(openvr_data::WebConsole);
	BIND_ENUM_CONSTANT(openvr_data::InputBindingLoad);
	BIND_ENUM_CONSTANT(openvr_data::InputActionManifestLoad);
	BIND_ENUM_CONSTANT(openvr_data::SpatialAnchor);
	BIND_ENUM_CONSTANT(openvr_data::ProgressUpdate);
	BIND_ENUM_CONSTANT(openvr_data::ShowUI);
	BIND_ENUM_CONSTANT(openvr_data::ShowDevTools);
	BIND_ENUM_CONSTANT(openvr_data::HDCPError);
	BIND_ENUM_CONSTANT(openvr_data::AudioVolumeControl);
	BIND_ENUM_CONSTANT(openvr_data::AudioMuteControl);

	// The EVREvent "documentation" is incomplete: only a handful of the events have a comment describing which VREvent_Data_t
	// is attached to them. Here, we use `Unknown` for the ones that haven't yet been reverse engineered. `None` means it is
	// known to actually not include any data.
	//
	// Any entries which are determined via reverse engineering (or guessing) should have a comment indicating as much, just
	// to set the appropriate level of skepticism if bugs are traced back here.
	//
	// The last argument of the macro is provided to track the confidence in an entry being correct:
	// - "none" means it's a mystery, should appear with Unknown.
	// - "guess" means makes sense but hasn't been tested
	// - "tested" means it was a guess but has been tested and seems to return the correct info
	// - "header" means openvr.h says so
	// - "comment" means see the comment for details
	//
	// These values are only for human reference. Feel free to add another if it's more nuanced.
	//
	// The whitespace below matches Valve's so this is a little easier to skim alongside openvr.h. Overlay events are left
	// here as comments for completeness, but actually exist in openvr_overlay_container.cpp. Other events that are sent only
	// to the compositor, etc, are also preserved as comments for completeness.

	VREVENT_SIGNAL(vr::EVREventType::VREvent_TrackedDeviceActivated, openvr_data::None, guess); // Guessing None because the trackedDeviceIndex in the event would identify which one.
	VREVENT_SIGNAL(vr::EVREventType::VREvent_TrackedDeviceDeactivated, openvr_data::None, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_TrackedDeviceUpdated, openvr_data::None, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_TrackedDeviceUserInteractionStarted, openvr_data::None, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_TrackedDeviceUserInteractionEnded, openvr_data::None, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_IpdChanged, openvr_data::Ipd, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_EnterStandbyMode, openvr_data::None, guess); // Probably no info needed.
	VREVENT_SIGNAL(vr::EVREventType::VREvent_LeaveStandbyMode, openvr_data::None, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_TrackedDeviceRoleChanged, openvr_data::None, comment); // https://github.com/ValveSoftware/openvr/issues/853: No data OR trackedDeviceIndex, re-query all devices.
	VREVENT_SIGNAL(vr::EVREventType::VREvent_WatchdogWakeUpRequested, openvr_data::None, guess); // Probably no info needed.
	VREVENT_SIGNAL(vr::EVREventType::VREvent_LensDistortionChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_PropertyChanged, openvr_data::Property, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_WirelessDisconnect, openvr_data::None, guess); // trackedDeviceIndex again
	VREVENT_SIGNAL(vr::EVREventType::VREvent_WirelessReconnect, openvr_data::None, guess);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_ButtonPress, openvr_data::Controller, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_ButtonUnpress, openvr_data::Controller, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_ButtonTouch, openvr_data::Controller, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_ButtonUntouch, openvr_data::Controller, header);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_Modal_Cancel, openvr_data::Unknown, none);

	// vr::EVREventType::VREvent_MouseMove
	// vr::EVREventType::VREvent_MouseButtonDown
	// vr::EVREventType::VREvent_MouseButtonUp
	// vr::EVREventType::VREvent_FocusEnter
	// vr::EVREventType::VREvent_FocusLeave
	// vr::EVREventType::VREvent_ScrollDiscrete
	// vr::EVREventType::VREvent_TouchPadMove
	VREVENT_SIGNAL(vr::EVREventType::VREvent_OverlayFocusChanged, openvr_data::Overlay, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_ReloadOverlays, openvr_data::Unknown, none);
	// vr::EVREventType::VREvent_ScrollSmooth
	// vr::EVREventType::VREvent_LockMousePosition
	// vr::EVREventType::VREvent_UnlockMousePosition

	VREVENT_SIGNAL(vr::EVREventType::VREvent_InputFocusCaptured, openvr_data::Process, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_InputFocusReleased, openvr_data::Process, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_SceneApplicationChanged, openvr_data::Process, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_InputFocusChanged, openvr_data::Process, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_SceneApplicationUsingWrongGraphicsAdapter, openvr_data::Process, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_ActionBindingReloaded, openvr_data::Process, header);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_HideRenderModels, openvr_data::None, guess); // No more info seems needed
	VREVENT_SIGNAL(vr::EVREventType::VREvent_ShowRenderModels, openvr_data::None, guess);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_SceneApplicationStateChanged, openvr_data::None, header);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_SceneAppPipeDisconnected, openvr_data::Process, header);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_ConsoleOpened, openvr_data::WebConsole, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_ConsoleClosed, openvr_data::WebConsole, guess);

	// vr::EVREventType::VREvent_OverlayShown
	// vr::EVREventType::VREvent_OverlayHidden
	VREVENT_SIGNAL(vr::EVREventType::VREvent_DashboardActivated, openvr_data::None, tested);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_DashboardDeactivated, openvr_data::None, tested);
	// vr::EVREventType::VREvent_DashboardRequested
	// vr::EVREventType::VREvent_ResetDashboard
	// vr::EVREventType::VREvent_ImageLoaded
	// vr::EVREventType::VREvent_ShowKeyboard
	// vr::EVREventType::VREvent_HideKeyboard
	// vr::EVREventType::VREvent_OverlayGamepadFocusGained
	// vr::EVREventType::VREvent_OverlayGamepadFocusLost
	VREVENT_SIGNAL(vr::EVREventType::VREvent_OverlaySharedTextureChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_ScreenshotTriggered, openvr_data::Unknown, none);
	// vr::EVREventType::VREvent_ImageFailed
	VREVENT_SIGNAL(vr::EVREventType::VREvent_DashboardOverlayCreated, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_SwitchGamepadFocus, openvr_data::Unknown, none);

	// vr::EVREventType::VREvent_RequestScreenshot
	VREVENT_SIGNAL(vr::EVREventType::VREvent_ScreenshotTaken, openvr_data::Screenshot, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_ScreenshotFailed, openvr_data::Screenshot, guess);
	// vr::EVREventType::VREvent_SubmitScreenshotToDashboard
	// vr::EVREventType::VREvent_ScreenshotProgressToDashboard

	VREVENT_SIGNAL(vr::EVREventType::VREvent_PrimaryDashboardDeviceChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_RoomViewShown, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_RoomViewHidden, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_ShowUI, openvr_data::ShowUI, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_ShowDevTools, openvr_data::ShowDevTools, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_DesktopViewUpdating, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_DesktopViewReady, openvr_data::Unknown, none);

	// vr::EVREventType::VREvent_StartDashboard
	// vr::EVREventType::VREvent_ElevatePrism

	// vr::EVREventType::VREvent_OverlayClosed

	VREVENT_SIGNAL(vr::EVREventType::VREvent_DashboardThumbChanged, openvr_data::Overlay, tested);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_DesktopMightBeVisible, openvr_data::None, guess); // A global state
	VREVENT_SIGNAL(vr::EVREventType::VREvent_DesktopMightBeHidden, openvr_data::None, guess);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_MutualSteamCapabilitiesChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_OverlayCreated, openvr_data::Overlay, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_OverlayDestroyed, openvr_data::Overlay, header);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_Notification_Shown, openvr_data::Notification, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_Notification_Hidden, openvr_data::Notification, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_Notification_BeginInteraction, openvr_data::Notification, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_Notification_Destroyed, openvr_data::Notification, guess);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_Quit, openvr_data::Process, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_ProcessQuit, openvr_data::Process, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_QuitAcknowledged, openvr_data::Process, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_DriverRequestedQuit, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_RestartRequested, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_InvalidateSwapTextureSets, openvr_data::Unknown, none);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_ChaperoneDataHasChanged, openvr_data::Chaperone, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_ChaperoneUniverseHasChanged, openvr_data::Chaperone, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_ChaperoneTempDataHasChanged, openvr_data::Chaperone, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_ChaperoneSettingsHaveChanged, openvr_data::Chaperone, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_SeatedZeroPoseReset, openvr_data::SeatedZeroPoseReset, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_ChaperoneFlushCache, openvr_data::Chaperone, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_ChaperoneRoomSetupStarting, openvr_data::Chaperone, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_ChaperoneRoomSetupFinished, openvr_data::Chaperone, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_StandingZeroPoseReset, openvr_data::Unknown, none);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_AudioSettingsHaveChanged, openvr_data::Unknown, none);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_BackgroundSettingHasChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_CameraSettingsHaveChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_ReprojectionSettingHasChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_ModelSkinSettingsHaveChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_EnvironmentSettingsHaveChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_PowerSettingsHaveChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_EnableHomeAppSettingsHaveChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_SteamVRSectionSettingChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_LighthouseSectionSettingChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_NullSectionSettingChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_UserInterfaceSectionSettingChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_NotificationsSectionSettingChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_KeyboardSectionSettingChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_PerfSectionSettingChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_DashboardSectionSettingChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_WebInterfaceSectionSettingChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_TrackersSectionSettingChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_LastKnownSectionSettingChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_DismissedWarningsSectionSettingChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_GpuSpeedSectionSettingChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_WindowsMRSectionSettingChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_OtherSectionSettingChanged, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_AnyDriverSettingsChanged, openvr_data::Unknown, none);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_StatusUpdate, openvr_data::Status, guess);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_WebInterface_InstallDriverCompleted, openvr_data::Unknown, none);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_MCImageUpdated, openvr_data::Unknown, none);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_FirmwareUpdateStarted, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_FirmwareUpdateFinished, openvr_data::Unknown, none);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_KeyboardClosed, openvr_data::None, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_KeyboardCharInput, openvr_data::Keyboard, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_KeyboardDone, openvr_data::None, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_KeyboardOpened_Global, openvr_data::Keyboard, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_KeyboardClosed_Global, openvr_data::Keyboard, header);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_ApplicationListUpdated, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_ApplicationMimeTypeLoad, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_ProcessConnected, openvr_data::Process, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_ProcessDisconnected, openvr_data::Process, guess);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_Compositor_ChaperoneBoundsShown, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_Compositor_ChaperoneBoundsHidden, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_Compositor_DisplayDisconnected, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_Compositor_DisplayReconnected, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_Compositor_HDCPError, openvr_data::HDCPError, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_Compositor_ApplicationNotResponding, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_Compositor_ApplicationResumed, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_Compositor_OutOfVideoMemory, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_Compositor_DisplayModeNotSupported, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_Compositor_StageOverrideReady, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_Compositor_RequestDisconnectReconnect, openvr_data::Unknown, none);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_TrackedCamera_StartVideoStream, openvr_data::None, guess); // trackedDeviceIndex probably indicates source
	VREVENT_SIGNAL(vr::EVREventType::VREvent_TrackedCamera_StopVideoStream, openvr_data::None, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_TrackedCamera_PauseVideoStream, openvr_data::None, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_TrackedCamera_ResumeVideoStream, openvr_data::None, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_TrackedCamera_EditingSurface, openvr_data::EditingCameraSurface, guess);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_PerformanceTest_EnableCapture, openvr_data::None, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_PerformanceTest_DisableCapture, openvr_data::None, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_PerformanceTest_FidelityLevel, openvr_data::PerformanceTest, guess); // fidelity is the only member of the struct

	VREVENT_SIGNAL(vr::EVREventType::VREvent_MessageOverlay_Closed, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_MessageOverlayCloseRequested, openvr_data::Unknown, none);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_Input_HapticVibration, openvr_data::HapticVibration, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_Input_BindingLoadFailed, openvr_data::InputBindingLoad, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_Input_BindingLoadSuccessful, openvr_data::InputBindingLoad, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_Input_ActionManifestReloaded, openvr_data::None, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_Input_ActionManifestLoadFailed, openvr_data::InputActionManifestLoad, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_Input_ProgressUpdate, openvr_data::ProgressUpdate, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_Input_TrackerActivated, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_Input_BindingsUpdated, openvr_data::Unknown, none);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_Input_BindingSubscriptionChanged, openvr_data::Unknown, none);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_SpatialAnchors_PoseUpdated, openvr_data::SpatialAnchor, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_SpatialAnchors_DescriptorUpdated, openvr_data::SpatialAnchor, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_SpatialAnchors_RequestPoseUpdate, openvr_data::SpatialAnchor, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_SpatialAnchors_RequestDescriptorUpdate, openvr_data::SpatialAnchor, header);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_SystemReport_Started, openvr_data::Unknown, none);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_Monitor_ShowHeadsetView, openvr_data::Process, header);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_Monitor_HideHeadsetView, openvr_data::Process, header);

	VREVENT_SIGNAL(vr::EVREventType::VREvent_Audio_SetSpeakersVolume, openvr_data::AudioVolumeControl, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_Audio_SetSpeakersMute, openvr_data::AudioMuteControl, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_Audio_SetMicrophoneVolume, openvr_data::AudioVolumeControl, guess);
	VREVENT_SIGNAL(vr::EVREventType::VREvent_Audio_SetMicrophoneMute, openvr_data::AudioMuteControl, guess);
}

void OpenVREventHandler::register_event_signal(uint32_t p_event_id, openvr_data::OpenVREventDataType p_type, String p_signal_name) {
	Dictionary eventAgeSeconds;
	eventAgeSeconds["name"] = "eventAgeSeconds";
	eventAgeSeconds["type"] = Variant::INT;
	Dictionary trackedDeviceIndex;
	trackedDeviceIndex["name"] = "trackedDeviceIndex";
	trackedDeviceIndex["type"] = Variant::INT;
	Dictionary data;
	data["name"] = "data";
	data["type"] = Variant::DICTIONARY;

	Array args;
	args.push_back(eventAgeSeconds);
	args.push_back(trackedDeviceIndex);
	args.push_back(data);

	this->add_user_signal(p_signal_name, args);
	openvr_data::register_event_signal(p_event_id, p_type, p_signal_name);
}

OpenVREventHandler::OpenVREventHandler() {
	ovr = openvr_data::retain_singleton();
	ovr->set_vrevent_handler(this);
}

OpenVREventHandler::~OpenVREventHandler() {
	ovr->remove_vrevent_handler(this);
	ovr->release();
}
