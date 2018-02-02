#include "blit_shader.h"

const char *const blit_vertex_shader =
		"#version 330\n"
		"\n"
		"layout (location=0) in vec2 coords;"
		"out vec2 T;"
		"\n"
		"void main(void)\n"
		"{\n"
		"T = coords;\n"
		"gl_Position = vec4((coords * 2.0) - 1.0, 0.0, 1.0);\n"
		"}";

const char *const blit_fragment_shader =
		"#version 330\n"
		"\n"
		"uniform sampler2D hdr_texture;\n"
		"\n"
		"in vec2 T;\n"
		"out vec4 color;\n"
		"\n"
		"void main()\n"
		"{\n"
		"color = texture(hdr_texture, T);\n"
		"}";

float blit_shader::vertice_data[12] = {
	0.0, 1.0,
	1.0, 0.0,
	0.0, 0.0,
	0.0, 1.0,
	1.0, 1.0,
	1.0, 0.0
};

void blit_shader::compile_shader(GLuint shader, const char *src) {
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);

	GLint status;
	GLint length;
	char log[4096] = { 0 };

	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	glGetShaderInfoLog(shader, 4096, &length, log);
	if (status == GL_FALSE) {
		printf("Compile failed %s\n", log);
	}
}

bool blit_shader::link_shader() {
	printf("Compiling blit shader\n");

	// Create the handels
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	program = glCreateProgram();

	// Attach the shaders to a program handel.
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	// Load and compile the Vertex Shader
	compile_shader(vertexShader, blit_vertex_shader);

	// Load and compile the Fragment Shader
	compile_shader(fragmentShader, blit_fragment_shader);

	printf("Linking blit shaders\n");
	glLinkProgram(program);

	// The shader objects are not needed any more,
	// the shader_program is the complete shader to be used.
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	GLint status;
	GLint length;
	char log[4096] = { 0 };

	glGetProgramiv(program, GL_LINK_STATUS, &status);
	glGetProgramInfoLog(program, 4096, &length, log);
	if (status == GL_FALSE) {
		printf("Link failed %s\n", log);

		glDeleteProgram(program);
		program = 0;
		return false;
	};

	// and set some properties that never change
	glUniform1i(glGetUniformLocation(program, "hdr_texture"), 0);
	glUseProgram(0);

	return true;
}

void blit_shader::render(GLuint p_texture_id) {
	if (initialise_on_first_use()) {
		// remember our current program
		GLuint was_program;
		glGetIntegerv(GL_CURRENT_PROGRAM, (GLint *)&was_program);

		// set our shader up
		glUseProgram(program);

		// set our texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, p_texture_id);

		// bind our vao to restore our state
		glBindVertexArray(vao);

		// render our rectangle
		glDrawArrays(GL_TRIANGLES, 0, 3 * 2);

		// and unbind
		glBindVertexArray(0);
		glUseProgram(was_program);
	}
}

bool blit_shader::initialise_on_first_use() {
	if (is_initialised) {
		// don't run this again, even if it failed		
	} else {
		// even if we fail, mark this as initialized
		is_initialised = true;

		// Create our shader program
		if (link_shader()) {
			// Need a Vertex Array Object
			glGenVertexArrays(1, &vao);

			// Bind our VAO, all relevant state changes are bound to our VAO, will be unset when we unbind, and reset when we bind...
			glBindVertexArray(vao);

			// Need a Vertex Buffer Object
			glGenBuffers(1, &vbo);

			// Now bind our Vertex Buffer Object and load up some data!
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, vertice_data, GL_STATIC_DRAW);

			// And setup our attributes
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (GLvoid *)0);

			// and unbind our vao to return back to our old state
			glBindVertexArray(0);
		}
	}

	return program != 0;
}


blit_shader::blit_shader() {
	program = 0;
	vao = 0;
	vbo = 0;
	is_initialised = false;
}

blit_shader::~blit_shader() {
	if (program != 0) {
		glDeleteProgram(program);
		program = 0;
	}

	if (vao != 0) {
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	if (vbo != 0) {
		glDeleteBuffers(1, &vbo);
		vbo = 0;
	}

	is_initialised = false;
}

