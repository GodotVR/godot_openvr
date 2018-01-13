////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes the render models available in OpenVR as resources to Godot

// Note, even though this is pure C code, we're using the C++ compiler as
// Microsoft never updated their C compiler to understand more modern dialects
// and openvr uses pesky things such as namespaces

#include "OVRRenderModel.h"

#include "ArrayMesh.h"
#include "SpatialMaterial.h"
#include "ImageTexture.h"
#include "Image.h"

typedef struct render_model_data_struct {
	openvr_data_struct *ovr;
} render_model_data_struct;

GDCALLINGCONV void *openvr_render_model_constructor(godot_object *p_instance, void *p_method_data) {
	render_model_data_struct *render_model_data;

	// log just for testing, should remove this
	printf("Creating render model resource\n");

	render_model_data = (render_model_data_struct *)api->godot_alloc(sizeof(render_model_data_struct));
	if (render_model_data != NULL) {
		render_model_data->ovr = openvr_get_data();
	}

	return render_model_data;
}

GDCALLINGCONV void openvr_render_model_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data) {
	// log just for testing, should remove this
	printf("Releasing render model resource\n");

	if (p_user_data != NULL) {
		render_model_data_struct *render_model_data = (render_model_data_struct *) p_user_data;
		if (render_model_data->ovr != NULL) {
			openvr_release_data();
			render_model_data->ovr = NULL;
		}
	}
}

GDCALLINGCONV godot_variant openvr_render_model_list(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	godot_variant ret;
	godot_array model_names;

	api->godot_array_new(&model_names);

	if (p_user_data != NULL) {
		render_model_data_struct *render_model_data = (render_model_data_struct *) p_user_data;
		if (render_model_data->ovr != NULL) {
			int model_count = render_model_data->ovr->render_models->GetRenderModelCount();
			for (int m = 0; m < model_count; m++) {
				godot_string s;
				godot_variant v;
				char model_name[256];
				render_model_data->ovr->render_models->GetRenderModelName(m, model_name, 256);
				api->godot_string_new(&s);
				api->godot_string_parse_utf8_with_len(&s, model_name, strlen(model_name));

				api->godot_variant_new_string(&v, &s);
				api->godot_array_push_back(&model_names, &v);

				// !BAS! Do I need to destroy v and s ?
			};
		};
	};

	api->godot_variant_new_array(&ret, &model_names);

	// !BAS! Do I need to destroy model_names?
	return ret;
}

GDCALLINGCONV godot_variant openvr_render_model_load(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	godot_variant ret;
	bool loaded = false;

	// clear out previously loaded surfaces
	int surfaces = ArrayMesh_get_surface_count(p_instance);
	for (int s = 0; s < surfaces; s++) {
		ArrayMesh_surface_remove(p_instance, 0);
	};

	// get our model from openvr
	if (p_user_data != NULL) {
		render_model_data_struct *render_model_data = (render_model_data_struct *) p_user_data;

		printf("Argument count: %i\n", p_num_args);
		if ((render_model_data->ovr != NULL) && (p_num_args > 0)) {
			godot_string find_name = api->godot_variant_as_string(p_args[0]);
			vr::RenderModel_t *ovr_render_model = NULL;
			vr::RenderModel_TextureMap_t *ovr_texture = NULL;

			godot_char_string find_cs = api->godot_string_ascii(&find_name);
			printf("Searching for: %s\n", api->godot_char_string_get_data(&find_cs));

			// Load our render model
			vr::EVRRenderModelError err = vr::VRRenderModelError_Loading;
			while (err == vr::VRRenderModelError_Loading) {
				err = render_model_data->ovr->render_models->LoadRenderModel_Async(api->godot_char_string_get_data(&find_cs), &ovr_render_model);
				ThreadSleep(1);
			};

			if (err != vr::VRRenderModelError_None) {
				printf("OpenVR: Couldn''t find model for %s (%i)\n", api->godot_char_string_get_data(&find_cs), err);
			} else {
				godot_variant variant;
				godot_pool_vector3_array vertices;
				godot_pool_vector3_array normals;
				godot_pool_vector2_array texcoords;
				godot_pool_int_array indices;
				godot_array prim_array;
				godot_array blend_array;
				godot_object *material;
				godot_object *texture;
				godot_object *image;

				// copy our vertices
				api->godot_pool_vector3_array_new(&vertices);
				api->godot_pool_vector3_array_resize(&vertices, ovr_render_model->unVertexCount);
				api->godot_pool_vector3_array_new(&normals);
				api->godot_pool_vector3_array_resize(&normals, ovr_render_model->unVertexCount);
				api->godot_pool_vector2_array_new(&texcoords);
				api->godot_pool_vector2_array_resize(&texcoords, ovr_render_model->unVertexCount);
				for (int i = 0; i < ovr_render_model->unVertexCount; i++) {
					godot_vector3 v, n;
					godot_vector2 t;

					api->godot_vector3_new(&v, ovr_render_model->rVertexData[i].vPosition.v[0], ovr_render_model->rVertexData[i].vPosition.v[1], ovr_render_model->rVertexData[i].vPosition.v[2]);
					api->godot_pool_vector3_array_set(&vertices, i, &v);

					api->godot_vector3_new(&n, ovr_render_model->rVertexData[i].vNormal.v[0], ovr_render_model->rVertexData[i].vNormal.v[1], ovr_render_model->rVertexData[i].vNormal.v[2]);
					api->godot_pool_vector3_array_set(&normals, i, &n);

					api->godot_vector2_new(&t, ovr_render_model->rVertexData[i].rfTextureCoord[0], ovr_render_model->rVertexData[i].rfTextureCoord[1]);
					api->godot_pool_vector2_array_set(&texcoords, i, &t);
				};

				// copy our indices, for some reason these are other way around :)
				api->godot_pool_int_array_new(&indices);
				api->godot_pool_int_array_resize(&indices, ovr_render_model->unTriangleCount * 3);
				for (int i = 0; i < ovr_render_model->unTriangleCount * 3; i += 3) {
					api->godot_pool_int_array_set(&indices, i+0, ovr_render_model->rIndexData[i+2]);
					api->godot_pool_int_array_set(&indices, i+1, ovr_render_model->rIndexData[i+1]);
					api->godot_pool_int_array_set(&indices, i+2, ovr_render_model->rIndexData[i+0]);
				};

				// create our array for our model
				api->godot_array_new(&prim_array);
				api->godot_array_new(&blend_array);
				api->godot_array_resize(&prim_array, ARRAY_MAX);

				// load our pool arrays into our array
				api->godot_variant_new_pool_vector3_array(&variant, &vertices);
				api->godot_array_set(&prim_array, ARRAY_VERTEX, &variant);
				api->godot_variant_destroy(&variant);
				api->godot_variant_new_pool_vector3_array(&variant, &normals);
				api->godot_array_set(&prim_array, ARRAY_NORMAL, &variant);
				api->godot_variant_destroy(&variant);
				api->godot_variant_new_pool_vector2_array(&variant, &texcoords);
				api->godot_array_set(&prim_array, ARRAY_TEX_UV, &variant);
				api->godot_variant_destroy(&variant);
				api->godot_variant_new_pool_int_array(&variant, &indices);
				api->godot_array_set(&prim_array, ARRAY_INDEX, &variant);
				api->godot_variant_destroy(&variant);

				// and load
				ArrayMesh_add_surface_from_arrays(p_instance, PRIMITIVE_TRIANGLES, prim_array, blend_array, ARRAY_COMPRESS_DEFAULT);

				// load our texture
				err = vr::VRRenderModelError_Loading;
				while (err == vr::VRRenderModelError_Loading) {
					err = vr::VRRenderModels()->LoadTexture_Async(ovr_render_model->diffuseTextureId, &ovr_texture);
					ThreadSleep( 1 );
				};

				if (err != vr::VRRenderModelError_None) {
					printf("Couldn''t load texture for render model\n");
				} else {
					godot_pool_byte_array image_data;
					api->godot_pool_byte_array_new(&image_data);
					api->godot_pool_byte_array_resize(&image_data, ovr_texture->unWidth * ovr_texture->unHeight * 4);

					// !BAS! There has to be a better way, this is going to be SLOOOOOW.....
					for (int i = 0; i < ovr_texture->unWidth * ovr_texture->unHeight * 4; i++) {
						api->godot_pool_byte_array_set(&image_data, i, ovr_texture->rubTextureMapData[i]);
					};

					// Prepare our image
					image = Image_new();
					Image_create_from_data(image, ovr_texture->unWidth, ovr_texture->unHeight, false, FORMAT_RGBA8, &image_data);
					api->godot_pool_byte_array_destroy(&image_data);

					// Prepare our texture
					texture = ImageTexture_new();
					ImageTexture_create_from_image(texture, image, 7);

					// and prepare our material
					material = SpatialMaterial_new();
					SpatialMaterial_set_texture(material, TEXTURE_ALBEDO, texture);

					ArrayMesh_surface_set_material(p_instance, 0, material);
				};

				// and cleanup
				api->godot_array_destroy(&prim_array);
				api->godot_array_destroy(&blend_array);
				api->godot_pool_vector3_array_destroy(&vertices);
				api->godot_pool_vector3_array_destroy(&normals);
				api->godot_pool_vector2_array_destroy(&texcoords);
				api->godot_pool_int_array_destroy(&indices);
				// do we cleanup image, texture and material?

				render_model_data->ovr->render_models->FreeRenderModel(ovr_render_model);
				if (ovr_texture != NULL) {
					// destroy this?
				};
				loaded = true;
			};

			api->godot_char_string_destroy(&find_cs);
			api->godot_string_destroy(&find_name);
		};
	};

	api->godot_variant_new_bool(&ret, loaded);
	return ret;
};

