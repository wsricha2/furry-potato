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

GLuint	program;
GLuint	texture_id;
GLuint	vbo_triangle,
				index_buffer,
				UV_buffer,
				normals_buffer;
GLint		attribute_coord4d,
				attribute_UV_coords,
				attribute_normals;
GLint		uniform_theta,
				uniform_model_view,
				uniform_projection,
				uniform_light,
				uniform_my_texture,
				uniform_UV_coords;
GLfloat	u_theta[] = { 0.0, 0.0, 0.0 };
GLfloat	u_light[] = { 0.0, 0.0, 1.0 };
GLint		u_my_texture;
GLfloat	rotation_velocity = 0.0,
				x_light_velocity = 0.0f,
				y_light_velocity = 0.0f;

glm::mat4 model_view_M = glm::lookAt(
  glm::vec3(  0.0,  0.0,  2.0 ),  //Eye
  glm::vec3(  0.0,  0.0,  0.0 ),  //At
  glm::vec3(  0.0,  1.0,  0.0 )   //Up
);

glm::mat4 projection_M = glm::perspective(
  glm::radians( 90.0f ),  //FOV
  1.0f * 640 / 480,       //Ratio
  0.1f,                   //Near
  10.0f                   //Far
);

void free_resources(){
  glDeleteProgram( program );
  glDeleteBuffers( 1, &vbo_triangle );
  glDeleteBuffers( 1, &normals_buffer );
	glDeleteTextures( 1, &texture_id );
};

bool init_resources(void){
  FILE *file_handle = fopen( "square.3d", "rb" );
	GLfloat *triangle_vertices;
	int triangle_count;
	
	if( file_handle != NULL ){
		fread( &triangle_count, sizeof( triangle_count ), 1, file_handle );
		triangle_vertices = (GLfloat*) malloc( triangle_count * sizeof( GLfloat ) * 12 );
		
		//Vertex Buffer ----------------------------------
		int index;
		for( index = 0; index < triangle_count; index ++ ){
			GLfloat *mesh_current;
			mesh_current = triangle_vertices + ( index * 12 );
			
			fread(   mesh_current       , sizeof( float ), 1, file_handle );
			fread( ( mesh_current + 1  ), sizeof( float ), 1, file_handle );
			fread( ( mesh_current + 2  ), sizeof( float ), 1, file_handle );
			fread( ( mesh_current + 3  ), sizeof( float ), 1, file_handle );
			fread( ( mesh_current + 4  ), sizeof( float ), 1, file_handle );
			fread( ( mesh_current + 5  ), sizeof( float ), 1, file_handle );
			fread( ( mesh_current + 6  ), sizeof( float ), 1, file_handle );
			fread( ( mesh_current + 7  ), sizeof( float ), 1, file_handle );
			fread( ( mesh_current + 8  ), sizeof( float ), 1, file_handle );
			fread( ( mesh_current + 9  ), sizeof( float ), 1, file_handle );
			fread( ( mesh_current + 10 ), sizeof( float ), 1, file_handle );
			fread( ( mesh_current + 11 ), sizeof( float ), 1, file_handle );
		}
	  glGenBuffers( 1, &vbo_triangle );
		glBindBuffer( GL_ARRAY_BUFFER, vbo_triangle );
		glBufferData( GL_ARRAY_BUFFER, triangle_count * sizeof( GLfloat ) * 12, triangle_vertices, GL_DYNAMIC_DRAW );
		free( triangle_vertices );
		fclose( file_handle );

		//index buffer -----------------------------------
		GLushort *tri_elements;
		tri_elements = ( GLushort* ) malloc( triangle_count * sizeof( GLushort ) * 3 );
		for( index = 0; index < ( triangle_count * 3 ); index++ ){
			*( tri_elements + index ) = index;
		}
		glGenBuffers( 1, &index_buffer );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, index_buffer );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, triangle_count * sizeof( GLushort ) * 3, tri_elements, GL_DYNAMIC_DRAW );
		free( tri_elements );
	}
	else{
		printf( "File failed to load\n" );
		return( false );
	}
	//Texture -----------------------------------------------------
	SDL_Surface *res_texture = IMG_Load( "map_0.png" );
	if( res_texture == NULL ){
		cerr << "IMG_Load: " << SDL_GetError() << endl;
		return( false );
	}
	glGenTextures( 1, &texture_id );
	glBindTexture( GL_TEXTURE_2D, texture_id );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, //target
								0, //level, 0 = base, no minimap
								GL_RGBA, // internalformat
								res_texture->w, //width
								res_texture->h, //height
								0, //border, always 0 in OpenGL ES
								GL_RGBA, //format
								GL_UNSIGNED_BYTE, //type
								res_texture->pixels );
	SDL_FreeSurface( res_texture );
	GLfloat UV_coords[] = {
		0.0, 1.0,
		0.0, 0.0,
		1.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
	};
	glGenBuffers( 1, &UV_buffer );
	glBindBuffer( GL_ARRAY_BUFFER, UV_buffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof( UV_coords ), UV_coords, GL_DYNAMIC_DRAW );
	//Normals -----------------------------------------------------
	/*
	GLfloat normals[] = {
		 0.000, 0.000, 1.000, 1.0,
		 0.000, 0.000, 1.000, 1.0,
		 0.000, 0.000, 1.000, 1.0,

		 0.000, 0.000, 1.000, 1.0,
		 0.000, 0.000, 1.000, 1.0,
		 0.000, 0.000, 1.000, 1.0,
	};
	glGenBuffers( 1, &normals_buffer );
	glBindBuffer( GL_ARRAY_BUFFER, normals_buffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof( normals ), &normals, GL_DYNAMIC_DRAW );
	*/
	
  //Attach glsl files -------------------------------------------
  GLint link_ok = GL_FALSE;
  
  GLuint vs, fs;
  if( (vs = create_shader( "tri.v.glsl", GL_VERTEX_SHADER   )) == 0){
    return( false );
  }
  if( (fs = create_shader( "tri.f.glsl", GL_FRAGMENT_SHADER )) == 0){
    return( false );
  }
  
  //Program and link check --------------------------------------
  program = glCreateProgram();
  glAttachShader( program, vs );
  glAttachShader( program, fs );
  glLinkProgram( program );
  glGetProgramiv( program, GL_LINK_STATUS, &link_ok );
  if( !link_ok ){
    cerr << "glLinkProgram:";
    print_log( program );
    return( false );
  }
  
 //Attributes ---------------------------------------------------
  const char* attribute_name = "v_position";
  attribute_coord4d = glGetAttribLocation( program, attribute_name );
  if( attribute_coord4d == -1 ){
    cerr << "Could not bind attribute " << attribute_name << endl;
    return( false );
  }
	/*
	attribute_name = "v_normals";
	attribute_normals = glGetAttribLocation( program, attribute_name );
	if( attribute_normals == -1 ){
		cerr << "Could not bind attribute " << attribute_name << endl;
		return( false );
	}
	*/
	attribute_name = "UV_coords";
	attribute_UV_coords = glGetAttribLocation( program, attribute_name );
	if( attribute_UV_coords == -1 ){
		cerr << "Could not bind attribute " << attribute_name << endl;
		return( false );
	}
  //Uniforms ----------------------------------------------------
  const char* uniform_name = "light";
  uniform_light = glGetUniformLocation( program, uniform_name );
  if( uniform_light == -1 ){
    cerr << "Could not bind uniform " << uniform_name << endl;
    return( false );
  }
  uniform_name = "model_view";
  uniform_model_view = glGetUniformLocation( program, uniform_name );
  if( uniform_model_view == -1 ){
    cerr << "Could not bind uniform " << uniform_name << endl;
    return( false );
  }
  uniform_name = "projection";
  uniform_projection = glGetUniformLocation( program, uniform_name );
  if( uniform_projection == -1 ){
    cerr << "Could not bind uniform " << uniform_name << endl;
    return( false );
  }
	
	return( true );
};

void render(SDL_Window* window){
  glClearColor( 0.5, 0.5, 0.5, 1.0 );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	u_light[0] += x_light_velocity;
	u_light[1] += y_light_velocity;
	
  glUseProgram( program );
  glEnableVertexAttribArray( attribute_coord4d );
  glBindBuffer( GL_ARRAY_BUFFER, vbo_triangle );
  glVertexAttribPointer( attribute_coord4d,
                         4,
                         GL_FLOAT,
                         GL_FALSE,
                         0,
                         0 );
	glEnableVertexAttribArray( attribute_UV_coords );
	glBindBuffer( GL_ARRAY_BUFFER, UV_buffer );
	glVertexAttribPointer( attribute_UV_coords,
												 2,
												 GL_FLOAT,
												 GL_FALSE,
												 0,
												 0 );
	/*
	glEnableVertexAttribArray( attribute_normals );
	glBindBuffer( GL_ARRAY_BUFFER, normals_buffer );
	glVertexAttribPointer( attribute_normals,
												 4,
												 GL_FLOAT,
												 GL_FALSE,
												 0,
												 0 );
	*/
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, index_buffer );
	glUniform3fv( uniform_light, 1, u_light );
  glUniformMatrix4fv( uniform_model_view, 1, GL_FALSE, glm::value_ptr( model_view_M ) );
  glUniformMatrix4fv( uniform_projection, 1, GL_FALSE, glm::value_ptr( projection_M ) );
	glActiveTexture( GL_TEXTURE0 );
	glUniform1i( uniform_my_texture, 0 );
	glBindTexture( GL_TEXTURE_2D, texture_id );
	//Render------------------------------------------------
  glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );
  glDisableVertexAttribArray( attribute_coord4d );
  
  SDL_GL_SwapWindow( window );
};

void mainloop(SDL_Window* window){
	char exit = 0;
	while( !exit ){
		SDL_Event event;
		while( SDL_PollEvent(&event) ){
			
			switch( event.type ){
				case SDL_KEYDOWN:
					switch( event.key.keysym.sym ){
						case SDLK_q:
							if( event.key.keysym.mod & KMOD_RCTRL || event.key.keysym.mod & KMOD_LCTRL ){
								exit = 1;
							}
							break;
						case SDLK_LEFT:
							x_light_velocity = -0.1f;
							break;
						case SDLK_RIGHT:
							x_light_velocity =  0.1f;
							break;
						case SDLK_UP:
							y_light_velocity =  0.1f;
							break;
						case SDLK_DOWN:
							y_light_velocity = -0.1f;
							break;
						default:
							break;
					}
					break;
				case SDL_QUIT:
					exit = 1;
					break;
				case SDL_KEYUP:
					x_light_velocity = 0.0f;
					y_light_velocity = 0.0f;
					break;
				default:
					break;
			}
			
		}
		
		render( window );
	}
};

int main(int argc, char*argv[]){
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("Triangle",
																				SDL_WINDOWPOS_CENTERED,
																				SDL_WINDOWPOS_CENTERED,
																				640, 480,
																				SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
																				);
	if(window == NULL){
		cerr << "Error: can't create window: " << SDL_GetError() << endl;
		return(EXIT_FAILURE);
	}
	
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 1);
	if( SDL_GL_CreateContext( window ) == NULL ){
		cerr << "Error: SDL_GL_CreateContext: " << SDL_GetError() << endl;
		return(EXIT_FAILURE);
	}
	
	GLenum glew_status = glewInit();
	if( glew_status != GLEW_OK ){
		cerr << "Error: glewInit: " << glewGetErrorString( glew_status ) << endl;
		return EXIT_FAILURE;
	}
	if( !GLEW_VERSION_2_0 ){
		cerr << "Error: your graphic card does not support OpenGL 2.0" << endl;
		return(EXIT_FAILURE);
	}
	
	if( !init_resources() ){
		return EXIT_FAILURE;
	}
	
	glEnable( GL_BLEND );
	glEnable( GL_DEPTH_TEST );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	mainloop( window );
	
	free_resources();
	return EXIT_SUCCESS;
};
