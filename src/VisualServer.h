////////////////////////////////////////////////////////////////////////////////////////////////
// Just exposing an interface to the VisualServer reachable in GDNative
// Basically just stole the relevant functions from godot-cpp

#ifndef VS_H
#define VS_H

#include "GodotCalls.h"

class VisualServer {
private:
	godot_object *_owner;
	static VisualServer *_singleton;

public:
	static inline VisualServer *get_singleton() {
		if (!VisualServer::_singleton) {
			VisualServer::_singleton = new VisualServer();
		}
		return VisualServer::_singleton;
	}

	godot_rid texture_create();
	godot_rid texture_create_from_image(godot_object *image, const int64_t flags = 7);
	void texture_allocate(const godot_rid texture, const int64_t width, const int64_t height, const int64_t depth_3d, const int64_t format, const int64_t type, const int64_t flags = 7);
	int64_t texture_get_texid(const godot_rid texture) const;
	void free_rid(const godot_rid rid);

	VisualServer();
};

#endif /* !VS_H */
