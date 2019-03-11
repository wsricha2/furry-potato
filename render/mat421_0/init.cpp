//#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <glm/ext.hpp>
using namespace std;

#include <GL/glew.h>
#include <SDL.h>

#include "../resources/shader_utils.h"
#include "SDL_image.h"

struct TEXTURE_PROGRAM{
	GLuint	pre_program;
	GLint	attrib_pre_position;
	GLint	attrib_pre_UV;

	GLuint	program;
	GLuint	vertices;
	GLuint	indices;
	GLuint	UV_coordinates;
	GLint	attrib_position;
	GLint	size_position;
	
	GLuint	texture;
	GLuint	frame_buffer;

	GLint	attrib_UV;
	GLint	size_UV;

	TEXTURE_PROGRAM* next;
};

void free_resources()
{
};

TEXTURE_PROGRAM* init_frame_setup(
	 char* model
	,char* pre_vertex_shader
	,char* pre_fragment_shader
	,char* vertex_shader
	,char* fragment_shader
	,char* png_file
){
	
	FILE *file_handle = fopen(model, "rb");
	TEXTURE_PROGRAM* output;

	if (file_handle != NULL)
	{

		GLuint	pre_program;
		GLint	attrib_pre_position;
		GLint	attrib_pre_UV;

		GLuint	program;
		GLuint	vertices;
		GLuint	indices;
		GLuint	UV_coordinates;
		GLint	attrib_position;
		GLint	size_position;
	
		GLuint	texture;
		GLuint	frame_buffer;

		GLint	attrib_UV;
		GLint	size_UV;

		output = (TEXTURE_PROGRAM*) malloc(sizeof(TEXTURE_PROGRAM));
		GLfloat *triangle_vertices;
		int triangle_count;

		fread(&triangle_count, sizeof(triangle_count), 1, file_handle);
		triangle_vertices = (GLfloat *)malloc(triangle_count * sizeof(GLfloat) * 12);

		//Vertex Buffer ------------------------------------------------------------
		fread(triangle_vertices, sizeof(float) * 12 * triangle_count, 1, file_handle);
		glGenBuffers(1, &vertices);
		glBindBuffer(GL_ARRAY_BUFFER, vertices);
		glBufferData(GL_ARRAY_BUFFER, triangle_count * sizeof(GLfloat) * 12, triangle_vertices, GL_DYNAMIC_DRAW);
		free(triangle_vertices);
		fclose(file_handle);
		//Index buffer -----------------------------------
		int index;
		GLushort *tri_elements;
		tri_elements = (GLushort *) malloc(triangle_count * sizeof(GLushort) * 3);
		for (index = 0; index < (triangle_count * 3); index++)
		{
			*(tri_elements + index) = index;
		}
		glGenBuffers(1, &indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangle_count * sizeof(GLushort) * 3, tri_elements, GL_DYNAMIC_DRAW);
		free(tri_elements);

		//Texture -----------------------------------------------------
		SDL_Surface *res_texture = IMG_Load(png_file);
		if (res_texture == NULL) //Failure Case
		{
			cerr << "IMG_Load: " << SDL_GetError() << endl;
			return (NULL);
		}
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			res_texture->w,
			res_texture->h,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			res_texture->pixels );
		SDL_FreeSurface(res_texture);
		
		//UV Coords
		GLfloat UV_coords[] = {
			0.0, 1.0,
			0.0, 0.0,
			1.0, 0.0,
			1.0, 0.0,
			1.0, 1.0,
			0.0, 1.0
		};
		glGenBuffers(1, &(output->UV_coordinates));
		glBindBuffer(GL_ARRAY_BUFFER, output->UV_coordinates);
		glBufferData(GL_ARRAY_BUFFER, sizeof(UV_coords), UV_coords, GL_DYNAMIC_DRAW);
		output->size_UV = 2;

		//Attach glsl files for pre-rendering ------------------------------------
		GLint link_ok = GL_FALSE;
		GLuint vs, fs;
		if ((vs = create_shader(pre_vertex_shader, GL_VERTEX_SHADER)) == 0)
		{
			return (NULL);
		}
		if ((fs = create_shader(pre_fragment_shader, GL_FRAGMENT_SHADER)) == 0)
		{
			return (NULL);
		}
		//Program and link check for pre-rendering -------------------------------
		program = glCreateProgram();
		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
		if (!link_ok)
		{
			cerr << "glLinkProgram:";
			print_log(program);
			return (NULL);
		}

		//Attributes (post) ---------------------------------------------------
		const char *attribute_name = "v_position";
		output->attrib_position = glGetAttribLocation(output->program, attribute_name);
		if (output->attrib_position == -1)
		{
			cerr << "Could not bind attribute " << attribute_name << endl;
			return (NULL);
		}
		output->attrib_pre_position = glGetAttribLocation(output->pre_program, attribute_name);
		if (output->attrib_pre_position == -1)
		{
			cerr << "Could not bind attribute " << attribute_name << endl;
			return (NULL);
		}

		attribute_name = "UV_coords";
		output->attrib_UV = glGetAttribLocation(output->program, attribute_name);
		if (output->attrib_UV == -1)
		{
			cerr << "Could not bind attribute " << attribute_name << endl;
			return (NULL);
		}
		output->attrib_pre_UV = glGetAttribLocation(output->pre_program, attribute_name);
		if (output->attrib_pre_UV == -1)
		{
			cerr << "Could not bind attribute " << attribute_name << endl;
			return (NULL);
		}
		//Uniforms

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, render_target);
		glViewport(0, 0, 640, 640);
		glClear(GL_DEPTH_BUFFER_BIT);

		glUseProgram(model->pre_program);
		glEnableVertexAttribArray(model->attrib_pre_position);
		glBindBuffer(GL_ARRAY_BUFFER, model->vertices);
		glVertexAttribPointer(model->attrib_pre_position,
							model->size_position,
							GL_FLOAT,
							GL_FALSE,
							0,
							0);
		glEnableVertexAttribArray(model->attrib_pre_UV);
		glBindBuffer(GL_ARRAY_BUFFER, model->UV_coordinates);
		glVertexAttribPointer(model->attrib_pre_UV,
							model->size_UV,
							GL_FLOAT,
							GL_FALSE,
							0,
							0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->indices);
		glBindTexture(GL_TEXTURE_2D, model->texture);
		glActiveTexture(GL_TEXTURE0);
		//Render------------------------------------------------
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
		glDisableVertexAttribArray(model->attrib_pre_position);
		glDisableVertexAttribArray(model->attrib_pre_position);
		glDisableVertexAttribArray(model->attrib_pre_UV);
	}
	else
	{
		printf("File failed to load\n");
		return(NULL);
	}

	return(output);
};


TEXTURE_PROGRAM* init_texture_resources(
	 char* model
	,char* pre_vertex_shader
	,char* pre_fragment_shader
	,char* vertex_shader
	,char* fragment_shader
//	,char* png_file
	)
{
	FILE *file_handle = fopen(model, "rb");
	TEXTURE_PROGRAM* output;

	if (file_handle != NULL)
	{
		output = (TEXTURE_PROGRAM*) malloc(sizeof(TEXTURE_PROGRAM));
		GLfloat *triangle_vertices;
		int triangle_count;

		fread(&triangle_count, sizeof(triangle_count), 1, file_handle);
		triangle_vertices = (GLfloat *)malloc(triangle_count * sizeof(GLfloat) * 12);

		//Vertex Buffer ------------------------------------------------------------
		fread(triangle_vertices, sizeof(float) * 12 * triangle_count, 1, file_handle);

		glGenBuffers(1, &(output->vertices));
		glBindBuffer(GL_ARRAY_BUFFER, output->vertices);
		glBufferData(GL_ARRAY_BUFFER, triangle_count * sizeof(GLfloat) * 12, triangle_vertices, GL_DYNAMIC_DRAW);
		free(triangle_vertices);
		fclose(file_handle);
		output->size_position = 4;

		//Index buffer -----------------------------------
		int index;
		GLushort *tri_elements;
		tri_elements = (GLushort *) malloc(triangle_count * sizeof(GLushort) * 3);
		for (index = 0; index < (triangle_count * 3); index++)
		{
			*(tri_elements + index) = index;
		}
		glGenBuffers(1, &(output->indices));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, output->indices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangle_count * sizeof(GLushort) * 3, tri_elements, GL_DYNAMIC_DRAW);
		free(tri_elements);

		//Texture -----------------------------------------------------
		/*
		SDL_Surface *res_texture = IMG_Load(png_file);
		if (res_texture == NULL) //Failure Case
		{
			cerr << "IMG_Load: " << SDL_GetError() << endl;
			return (NULL);
		}
		//*/
		glGenTextures(1, &(output->texture));
		glBindTexture(GL_TEXTURE_2D, output->texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			640,
			640,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			NULL);
//		SDL_FreeSurface(res_texture);

		//Frame Buffer -----------------------------------------------
		glGenFramebuffers(1, &(output->frame_buffer));
		glBindFramebuffer(GL_FRAMEBUFFER, output->frame_buffer);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D,
			output->texture,
			0
		);
		
		//UV Coords
		GLfloat UV_coords[] = {
			0.0, 1.0,
			0.0, 0.0,
			1.0, 0.0,
			1.0, 0.0,
			1.0, 1.0,
			0.0, 1.0
		};
		glGenBuffers(1, &(output->UV_coordinates));
		glBindBuffer(GL_ARRAY_BUFFER, output->UV_coordinates);
		glBufferData(GL_ARRAY_BUFFER, sizeof(UV_coords), UV_coords, GL_DYNAMIC_DRAW);
		output->size_UV = 2;

		//Attach glsl files for pre-rendering ------------------------------------
		GLint link_ok = GL_FALSE;
		GLuint vs, fs;
		if ((vs = create_shader(pre_vertex_shader, GL_VERTEX_SHADER)) == 0)
		{
			return (NULL);
		}
		if ((fs = create_shader(pre_fragment_shader, GL_FRAGMENT_SHADER)) == 0)
		{
			return (NULL);
		}
		//Program and link check for pre-rendering -------------------------------
		output->pre_program = glCreateProgram();
		glAttachShader(output->pre_program, vs);
		glAttachShader(output->pre_program, fs);
		glLinkProgram(output->pre_program);
		glGetProgramiv(output->pre_program, GL_LINK_STATUS, &link_ok);
		if (!link_ok)
		{
			cerr << "glLinkProgram:";
			print_log(output->pre_program);
			return (NULL);
		}

		//Attach glsl files fpr post-render --------------------------------------
		//GLint link_ok = GL_FALSE;
		//GLuint vs, fs;
		if ((vs = create_shader(vertex_shader, GL_VERTEX_SHADER)) == 0)
		{
			return (NULL);
		}
		if ((fs = create_shader(fragment_shader, GL_FRAGMENT_SHADER)) == 0)
		{
			return (NULL);
		}
		//Program and link check --------------------------------------
		output->program = glCreateProgram();
		glAttachShader(output->program, vs);
		glAttachShader(output->program, fs);
		glLinkProgram(output->program);
		glGetProgramiv(output->program, GL_LINK_STATUS, &link_ok);
		if (!link_ok)
		{
			cerr << "glLinkProgram:";
			print_log(output->program);
			return (NULL);
		}

		//Attributes (post) ---------------------------------------------------
		const char *attribute_name = "v_position";
		output->attrib_position = glGetAttribLocation(output->program, attribute_name);
		if (output->attrib_position == -1)
		{
			cerr << "Could not bind attribute " << attribute_name << endl;
			return (NULL);
		}
		output->attrib_pre_position = glGetAttribLocation(output->pre_program, attribute_name);
		if (output->attrib_pre_position == -1)
		{
			cerr << "Could not bind attribute " << attribute_name << endl;
			return (NULL);
		}

		attribute_name = "UV_coords";
		output->attrib_UV = glGetAttribLocation(output->program, attribute_name);
		if (output->attrib_UV == -1)
		{
			cerr << "Could not bind attribute " << attribute_name << endl;
			return (NULL);
		}
		output->attrib_pre_UV = glGetAttribLocation(output->pre_program, attribute_name);
		if (output->attrib_pre_UV == -1)
		{
			cerr << "Could not bind attribute " << attribute_name << endl;
			return (NULL);
		}
		//Uniforms
	}
	else
	{
		printf("File failed to load\n");
		return(NULL);
	}

	return(output);
};

//Render to window/screen
void render(SDL_Window *window, TEXTURE_PROGRAM* model)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 640, 640);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(model->program);
	glEnableVertexAttribArray(model->attrib_position);
	glBindBuffer(GL_ARRAY_BUFFER, model->vertices);
	glVertexAttribPointer(model->attrib_position,
						  model->size_position,
						  GL_FLOAT,
						  GL_FALSE,
						  0,
						  0);
	glEnableVertexAttribArray(model->attrib_UV);
	glBindBuffer(GL_ARRAY_BUFFER, model->UV_coordinates);
	glVertexAttribPointer(model->attrib_UV,
						  model->size_UV,
						  GL_FLOAT,
						  GL_FALSE,
						  0,
						  0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->indices);
	glBindTexture(GL_TEXTURE_2D, model->texture);
	glActiveTexture(GL_TEXTURE0);
	//Render------------------------------------------------
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	glDisableVertexAttribArray(model->attrib_position);
	glDisableVertexAttribArray(model->attrib_UV);

	SDL_GL_SwapWindow(window);
};

//Render to texture
void pre_render(GLuint render_target, TEXTURE_PROGRAM* model)
{	//This is for offscreen rendering
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, render_target);
	glViewport(0, 0, 640, 640);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(model->pre_program);
	glEnableVertexAttribArray(model->attrib_pre_position);
	glBindBuffer(GL_ARRAY_BUFFER, model->vertices);
	glVertexAttribPointer(model->attrib_pre_position,
						  model->size_position,
						  GL_FLOAT,
						  GL_FALSE,
						  0,
						  0);
	glEnableVertexAttribArray(model->attrib_pre_UV);
	glBindBuffer(GL_ARRAY_BUFFER, model->UV_coordinates);
	glVertexAttribPointer(model->attrib_pre_UV,
						  model->size_UV,
						  GL_FLOAT,
						  GL_FALSE,
						  0,
						  0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->indices);
	glBindTexture(GL_TEXTURE_2D, model->texture);
	glActiveTexture(GL_TEXTURE0);
	//Render------------------------------------------------
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	glDisableVertexAttribArray(model->attrib_pre_position);
	glDisableVertexAttribArray(model->attrib_pre_position);
	glDisableVertexAttribArray(model->attrib_pre_UV);
	
};

void mainloop(SDL_Window *window, TEXTURE_PROGRAM* model)
{
	char exit = 0;
	char next_frame = 0;
	while (!exit)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{

			switch (event.type)
			{
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_q:
					if (event.key.keysym.mod & KMOD_RCTRL || event.key.keysym.mod & KMOD_LCTRL)
					{
						exit = 1;
					}
					break;
				case SDLK_LEFT:
					break;
				case SDLK_RIGHT:
					next_frame = 1;
					break;
				case SDLK_UP:
					break;
				case SDLK_DOWN:
					break;
				default:
					next_frame = 0;
					break;
				}
				break;
			case SDL_QUIT:
				exit = 1;
				break;
			case SDL_KEYUP:
				next_frame = 0;
				break;
			default:
				next_frame = 0;
				break;
			}
		}
		if(next_frame == 1)
		{
			pre_render(model->next->frame_buffer, model);
			render(window, model->next);
			model = model->next;
			next_frame = 0;
		}
	}
};

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("Triangle",
										  SDL_WINDOWPOS_CENTERED,
										  SDL_WINDOWPOS_CENTERED,
										  640, 640,
										  SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	if (window == NULL)
	{
		cerr << "Error: can't create window: " << SDL_GetError() << endl;
		return (EXIT_FAILURE);
	}
	//----------------------------------------------------------------------------
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);
	if (SDL_GL_CreateContext(window) == NULL)
	{
		cerr << "Error: SDL_GL_CreateContext: " << SDL_GetError() << endl;
		return (EXIT_FAILURE);
	}
	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK)
	{
		cerr << "Error: glewInit: " << glewGetErrorString(glew_status) << endl;
		return EXIT_FAILURE;
	}
	if (!GLEW_VERSION_2_0)
	{
		cerr << "Error: your graphic card does not support OpenGL 2.0" << endl;
		return (EXIT_FAILURE);
	}
	//----------------------------------------------------------------------------

	TEXTURE_PROGRAM* checkers_head = init_texture_resources(
		 "square.3d"
		,"converge.v.glsl"
		,"converge.f.glsl"
		,"tri.v.glsl"
		,"tri.f.glsl"
//		,"map_0.png"
	);
	if (checkers_head == NULL)
	{
		return EXIT_FAILURE;
	}
	checkers_head->next = init_texture_resources(
		 "square.3d"
		,"converge.v.glsl"
		,"converge.f.glsl"
		,"tri.v.glsl"
		,"tri.f.glsl"
//		,"map_0.png"
	);
	if (checkers_head->next == NULL)
	{
		return EXIT_FAILURE;
	}
	checkers_head->next->next = checkers_head;

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	mainloop(window, checkers_head);

	free_resources();
	return EXIT_SUCCESS;
};