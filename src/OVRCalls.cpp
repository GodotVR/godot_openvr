////////////////////////////////////////////////////////////////////////////////////////////////
// Helper calls and singleton container for accessing openvr

// Note, even though this is pure C code, we're using the C++ compiler as
// Microsoft never updated their C compiler to understand more modern dialects
// and openvr uses pesky things such as namespaces

#if defined(_WIN32)
#include <windows.h>
#endif

#include "OVRCalls.h"

#if defined(POSIX)
#include "unistd.h"
#endif

openvr_data_struct *openvr_data_singleton = NULL;

void ThreadSleep( unsigned long nMilliseconds ) {
#if defined(_WIN32)
	Sleep( nMilliseconds );
#elif defined(POSIX)
	usleep( nMilliseconds * 1000 );
#endif
};

void openvr_release_data() {
	if (openvr_data_singleton == NULL) {
		// nothing to release
		printf("OpenVR: tried to release non-existant OpenVR context\n");
	} else if (openvr_data_singleton->use_count > 1) {
		// decrease use count
		openvr_data_singleton->use_count--;
		printf("OpenVR: decreased use count to %i\n", openvr_data_singleton->use_count);
	} else {
		// cleanup openvr
		printf("OpenVR: releasing OpenVR context\n");

		vr::VR_Shutdown();
		api->godot_free(openvr_data_singleton);
		openvr_data_singleton = NULL;
	};
};

openvr_data_struct *openvr_get_data() {
	if (openvr_data_singleton != NULL) {
		// increase use count
		openvr_data_singleton->use_count++;
		printf("OpenVR: increased use count to %i\n", openvr_data_singleton->use_count);
	} else {
		// init openvr
		printf("OpenVR: initialising OpenVR context\n");

		openvr_data_singleton = (openvr_data_struct *)api->godot_alloc(sizeof(openvr_data_struct));
		if (openvr_data_singleton != NULL) {
			bool success = true;
			vr::EVRInitError error = vr::VRInitError_None;

			openvr_data_singleton->use_count = 1;

			if (!vr::VR_IsRuntimeInstalled()) {
				printf("SteamVR has not been installed.\n");
				success = false;
			};

			if (success) {
				// Loading the SteamVR Runtime
				openvr_data_singleton->hmd = vr::VR_Init(&error, vr::VRApplication_Scene);

				if (error != vr::VRInitError_None) {
					success = false;
					printf("Unable to init VR runtime: %s\n", vr::VR_GetVRInitErrorAsEnglishDescription(error));
				} else {
					printf("Main OpenVR interface has been initialized\n");
				};
			};

			if (success) {
				// render models give us access to mesh representations of the various
				// controllers
				openvr_data_singleton->render_models = (vr::IVRRenderModels *)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &error);
				if (!openvr_data_singleton->render_models) {
					success = false;

					printf("Unable to get render model interface: %s\n", vr::VR_GetVRInitErrorAsEnglishDescription(error));
				} else {
					printf("Main render models interface has been initialized.\n");
				};
			};

			if (!vr::VRCompositor()) {
				success = false;

				printf("Compositor initialization failed. See log file for details.\n");
			};

			if (!success) {
				openvr_release_data();
			};
		};
	}

	return openvr_data_singleton;
};

char *openvr_get_device_name(openvr_data_struct *p_openvr_data, vr::TrackedDeviceIndex_t p_tracked_device_index, int pMaxLen) {
	static char returnstring[1025] = "Not initialised";

	// don't go bigger then this...
	if (pMaxLen > 1024) {
		pMaxLen = 1024;
	};

	if ((p_openvr_data->hmd != NULL) &&
			(p_tracked_device_index != vr::k_unTrackedDeviceIndexInvalid)) {
		uint32_t namelength = p_openvr_data->hmd->GetStringTrackedDeviceProperty(
				p_tracked_device_index, vr::Prop_RenderModelName_String, NULL, 0, NULL);
		if (namelength > 0) {
			if (namelength > pMaxLen) {
				namelength = pMaxLen;
			};

			p_openvr_data->hmd->GetStringTrackedDeviceProperty(
					p_tracked_device_index, vr::Prop_RenderModelName_String, returnstring,
					namelength, NULL);
		};
	};

	return returnstring;
};

void openvr_transform_from_matrix(godot_transform *p_dest, vr::HmdMatrix34_t *p_matrix, godot_real p_world_scale) {
	godot_basis basis;
	godot_vector3 origin;
	float *basis_ptr = (float *)&basis; // Godot can switch between real_t being
	// double or float.. which one is used...

	int k = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			basis_ptr[k++] = p_matrix->m[i][j];
		};
	};

	api->godot_vector3_new(&origin, p_matrix->m[0][3] * p_world_scale,
			p_matrix->m[1][3] * p_world_scale,
			p_matrix->m[2][3] * p_world_scale);
	api->godot_transform_new(p_dest, &basis, &origin);
};
