//#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <glm/ext.hpp>
#include <vector>
#include <sstream>
using namespace std;

#include <GL/glew.h>
#include <SDL.h>

#include "resources/shader_utils.h"

void load_obj_file(
	const char* filename,
	vector<glm::vec4> &vertices,
	vector<glm::vec3> &normals,
	vector<GLushort> &elements
){
	ifstream my_file(filename, ios::in);
	if(!my_file){
		cerr << "Could not open " << filename << endl;
		exit(1);
	}
	
	string line;
	while(getline(my_file, line)){
		if(line.substr(0, 2) == "v "){
			istringstream s(line.substr(2));
      glm::vec4 v;
			s >> v.x; s >> v.y; s >> v.z; v.w = 1.0f;
      vertices.push_back(v);
		}
		else if(line.substr(0, 2) == "f "){
			istringstream s(line.substr(2));
			GLushort a, b, c;
			s >> a; s >> b; s >> c;
			a --; b--; c--;
			elements.push_back(a);
			elements.push_back(b);
			elements.push_back(c);
		}
		else if(line.substr(0, 2) == "# "){}
		else{}
	}
};

struct PROGRAM{
	GLuint	program;
	GLuint	vertices;
	GLint	attrib_position;
	GLint	size_position;
	
	PROGRAM	*next;
};
PROGRAM *head = nullptr;

void free_resources(){
	PROGRAM* current = head;
	while(current != nullptr ){
		glDeleteProgram( current->program );
  		glDeleteBuffers( 1, &(current->vertices) );
		
		current = current->next;
		free(head);
		head = current;
	}
};

bool init_selection_box(SDL_Window* window, int x, int y){
	PROGRAM *temp;
	temp = (PROGRAM*) malloc(sizeof(PROGRAM));
	int w, h;
	SDL_GetWindowSize( window, &w, &h );
	GLfloat coords[2];
	coords[0] = ((GLfloat) x / (GLfloat) w) * 2.0f - 1.0f;
	coords[1] = ((GLfloat) -y / (GLfloat) h) * 2.0f + 1.0f;
	
	glGenBuffers( 1, &(temp->vertices) );
	glBindBuffer( GL_ARRAY_BUFFER, temp->vertices );
	glBufferData( GL_ARRAY_BUFFER, 2 * sizeof(GLfloat), coords, GL_DYNAMIC_DRAW );

	GLint link_ok = GL_FALSE;
	GLuint vs, fs;
	if( (vs = create_shader( "tri.v.glsl", GL_VERTEX_SHADER   )) == 0){
		return( false );
	}
	if( (fs = create_shader( "tri.f.glsl", GL_FRAGMENT_SHADER )) == 0){
		return( false );
	}

	temp->program = glCreateProgram();
	glAttachShader( temp->program, vs );
	glAttachShader( temp->program, fs );
	glLinkProgram( temp->program );
	glGetProgramiv( temp->program, GL_LINK_STATUS, &link_ok );
  	if( !link_ok ){
    	cerr << "glLinkProgram:";
    	print_log( temp->program );
    	return( false );
  	}
	const char* attribute_name = "v_position";
	temp->attrib_position = glGetAttribLocation( temp->program, attribute_name );
	if( temp->attrib_position == -1 ){
		cerr << "Could not bind attribute " << attribute_name << endl;
    	return( false );
	}
	temp->size_position = 2;

	temp->next = head;
	head = temp;
	return( true );
};

void render(SDL_Window* window){
	glClearColor( 0.5, 0.5, 0.5, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	PROGRAM *current = head;
	while( current != nullptr )
	{
		glUseProgram(current->program);
		glEnableVertexAttribArray(current->attrib_position);
		glBindBuffer( GL_ARRAY_BUFFER, current->vertices );
		glVertexAttribPointer(
			current->attrib_position,
			current->size_position,
			GL_FLOAT,
			GL_FALSE,
			0,
			0
		);

		glDrawArrays( GL_POINTS, 0, 1 );
  		glDisableVertexAttribArray( current->attrib_position );
		current = current->next;
	}
	SDL_GL_SwapWindow( window );
};

void mainloop(SDL_Window* window){
	char exit = 0;
	while( !exit ){
		SDL_Event event;
		
		while( SDL_PollEvent(&event) ){
			switch( event.type ){
				case SDL_WINDOWEVENT:
					if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
						glViewport(0, 0, event.window.data1, event.window.data2);
					}
					break;
				case SDL_KEYDOWN:
					switch( event.key.keysym.sym ){
						case SDLK_q:
							if( event.key.keysym.mod & KMOD_RCTRL || event.key.keysym.mod & KMOD_LCTRL ){
								exit = 1;
							}
							break;
						case SDLK_a:
							break;
						case SDLK_d:
							break;
						case SDLK_LEFT:
							break;
						case SDLK_RIGHT:
							break;
						case SDLK_UP:
							break;
						case SDLK_DOWN:
							break;
						default:
							break;
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					switch( event.button.button ){
						case SDL_BUTTON_LEFT:
							init_selection_box(window, event.motion.x, event.motion.y);
							break;
						default:
							break;
					}
					break;
				case SDL_QUIT:
					exit = 1;
					break;
				case SDL_KEYUP:
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
	SDL_Window* window = SDL_CreateWindow(
		"Software!",
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
	
	glEnable( GL_BLEND );
	glEnable( GL_DEPTH_TEST );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	mainloop( window );
	
	free_resources();
	return EXIT_SUCCESS;
};
