#include <cstdlib>
#include <iostream>
using namespace std;

#include <GL/glew.h>
#include <SDL.h>

GLuint program;
GLint attribute_coord2d;

bool init_resources(void){
	//Vertex
	GLint compile_ok = GL_FALSE, link_ok = GL_FALSE;
	GLuint vs = glCreateShader( GL_VERTEX_SHADER );
	const char *vs_source = "#version 120\n"
													"attribute vec2 coord2d; "
													"void main(void){ "
													"gl_Position = vec4(coord2d, 0.0, 1.0); "
													"}";
	glShaderSource( vs, 1, &vs_source, NULL );
	glCompileShader( vs );
	glGetShaderiv( vs, GL_COMPILE_STATUS, &compile_ok );
	if( !compile_ok ){
		cerr << "Error in the vertex shader" << endl;
		return false;
	}
	
	//Fragment
	GLuint fs = glCreateShader( GL_FRAGMENT_SHADER );
	const char *fs_source = "#version 120\n"
													"void main(void){ "
													"gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0); "
													"}";
	glShaderSource( fs, 1, &fs_source, NULL );
	glCompileShader( fs );
	glGetShaderiv( fs, GL_COMPILE_STATUS, &compile_ok );
	if( !compile_ok ){
		cerr << "Error in fragment shader" << endl;
		return false;
	}
	
	//
	program = glCreateProgram();
	glAttachShader( program, vs );
	glAttachShader( program, fs );
	glLinkProgram( program );
	glGetProgramiv( program, GL_LINK_STATUS, &link_ok );
	if( !link_ok ){
		cerr << "Error in glLinkProgram" << endl;
		return false;
	}
	
	//
	const char* attribute_name = "coord2d";
	attribute_coord2d = glGetAttribLocation( program, attribute_name );
	if( attribute_coord2d == -1 ){
		cerr << "Could not bind attribute " << attribute_name << endl;
		return false;
	}
	
	return true;
};

void render(SDL_Window* window){
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram( program );
	glEnableVertexAttribArray( attribute_coord2d );
	GLfloat triangle_vertices[] = {
		 0.0,  0.8,
		-0.8, -0.8,
		 0.8, -0.8,
	};
	
	glVertexAttribPointer( attribute_coord2d,
												 2,
												 GL_FLOAT,
												 GL_FALSE,
												 0,
												 triangle_vertices );
	glDrawArrays( GL_TRIANGLES, 0, 3 );
	glDisableVertexAttribArray( attribute_coord2d );
	
	SDL_GL_SwapWindow( window );
};

void free_resources(){
	glDeleteProgram( program );
};

void mainloop(SDL_Window* window){
	while( true ){
		SDL_Event event;
		while( SDL_PollEvent(&event) ){
			if( event.type == SDL_QUIT ){
				return;
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
	SDL_GL_CreateContext( window );
	
	GLenum glew_status = glewInit();
	if( glew_status != GLEW_OK ){
		cerr << "Error: glewInit: " << glewGetErrorString( glew_status ) << endl;
		return EXIT_FAILURE;
	}
	
	if( !init_resources() ){
		return EXIT_FAILURE;
	}
	
	mainloop( window );
	
	free_resources();
	return EXIT_SUCCESS;
};
