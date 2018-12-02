//#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <glm/ext.hpp>
using namespace std;

#include <GL/glew.h>
#include <SDL.h>

#include "../resources/shader_utils.h"

GLuint program;
GLuint vbo_triangle, index_buffer, UV_buffer;
GLint attribute_coord4d, attribute_UV_coords;
GLint uniform_theta, uniform_model_view, uniform_projection;
GLfloat u_theta[] = { 0.0, 0.0, 0.0 };
GLfloat u_light[] = { 2.0, 2.0, 2.0 };
GLfloat rotation_velocity = 0.0;

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
  glDeleteBuffers( 1, &index_buffer );
};

bool init_resources(void){
  FILE *file_handle = fopen( "hex_bead.3d", "rb" );
	GLfloat *triangle_vertices;
	int triangle_count;
	
	if( file_handle != NULL ){
		fread( &triangle_count, sizeof( triangle_count ), 1, file_handle );
		triangle_vertices = (GLfloat*) malloc( triangle_count * sizeof( GLfloat ) * 12 );
		
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
		glBufferData( GL_ARRAY_BUFFER, triangle_count * sizeof( GLfloat ) * 12, triangle_vertices, GL_STATIC_DRAW );
		free( triangle_vertices );
		fclose( file_handle );

		//index buffer
		GLushort *tri_elements;
		tri_elements = ( GLushort* ) malloc( triangle_count * sizeof( GLushort ) * 3 );
		for( index = 0; index < ( triangle_count * 3 ); index++ ){
			*( tri_elements + index ) = index;
		}
		glGenBuffers( 1, &index_buffer );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, index_buffer );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, triangle_count * sizeof( GLushort ) * 3, tri_elements, GL_STATIC_DRAW );
		free( tri_elements );
	}
	else{
		printf( "File failed to load\n" );
		return( false );
	}
  
  
  //Vertex
  GLint link_ok = GL_FALSE;
  
  GLuint vs, fs;
  if( (vs = create_shader( "tri.v.glsl", GL_VERTEX_SHADER   )) == 0){
    return( false );
  }
  if( (fs = create_shader( "tri.f.glsl", GL_FRAGMENT_SHADER )) == 0){
    return( false );
  }
  
  //Program and link check
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
  
 //Attributes
  const char* attribute_name = "v_position";
  attribute_coord4d = glGetAttribLocation( program, attribute_name );
  if( attribute_coord4d == -1 ){
    cerr << "Could not bind attribute " << attribute_name << endl;
    return( false );
  }
  //Uniforms
  const char* uniform_name = "theta";
  uniform_theta = glGetUniformLocation( program, uniform_name );
  if( uniform_theta == -1 ){
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
  
  u_theta[ 1 ] +=  rotation_velocity;
  if( u_theta[ 1 ] > 360.0f ){
    u_theta[ 1 ] -= 360.0f;
  }
	else if( u_theta[ 1 ] < 0.0f ){
		u_theta[ 1 ] += 360.0f;
	}
  
  glUseProgram( program );
  glEnableVertexAttribArray( attribute_coord4d );
  glBindBuffer( GL_ARRAY_BUFFER, vbo_triangle );
  glVertexAttribPointer( attribute_coord4d,
                         4,
                         GL_FLOAT,
                         GL_FALSE,
                         0,
                         0 );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, index_buffer );
  glUniform3fv( uniform_theta, 1, u_theta );
  glUniformMatrix4fv( uniform_model_view, 1, GL_FALSE, glm::value_ptr( model_view_M ) );
  glUniformMatrix4fv( uniform_projection, 1, GL_FALSE, glm::value_ptr( projection_M ) );
  glDrawElements( GL_TRIANGLES, 54, GL_UNSIGNED_SHORT, 0 );
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
							rotation_velocity =  1.0f;
							break;
						case SDLK_RIGHT:
							rotation_velocity = -1.0f;
							break;
						default:
							break;
					}
					break;
				case SDL_QUIT:
					exit = 1;
					break;
				case SDL_KEYUP:
					rotation_velocity = 0.0f;
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
