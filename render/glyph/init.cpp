#include <cstdlib>
#include <iostream>
using namespace std;

#include <GL/glew.h>
#include <SDL.h>

#include "SDL_image.h"
#include "../resources/shader_utils.h"
#include "init_resources.h"
#include "free_resources.h"
#include "render.h"

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
						default:
							break;
					}
					break;
				case SDL_QUIT:
					exit = 1;
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
