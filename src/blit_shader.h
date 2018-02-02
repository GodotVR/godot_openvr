/**
  simple blit shader
**/

#ifndef BLIT_SHADER_H
#define BLIT_SHADER_H

#include <glad/glad.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

class blit_shader {
private:
	GLuint program;
	GLuint vao;
	GLuint vbo;
	bool is_initialised;

	static float vertice_data[12];

	void compile_shader(GLuint shader, const char *src);
	bool link_shader();
public:
	blit_shader();
	~blit_shader();

	bool initialise_on_first_use();
	void render(GLuint p_texture_id);
};

#endif /* BLIT_SHADER_H */
