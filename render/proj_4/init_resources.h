GLuint program;
GLuint vbo_triangle, index_buffer, vbo_texcoords;
GLint attribute_coord2d, attribute_texcoord;
//Texture GLobals
GLuint texture_id, pogram_id;
GLint uniform_mytexture;

bool init_resources(void){
  GLfloat triangle_vertices[] = {
		 0.8, -0.8,  0.0,	//Bot-Right
		-0.8, -0.8,  0.0,	//Bot-Left
		-0.8,  0.8,  0.0,	//Top-Left
		-0.8,  0.8,  0.0,	//Top-Left
		 0.8,  0.8,  0.0,	//Top-Right
		 0.8, -0.8,  0.0,	//Bot-Right
  };
  glGenBuffers( 1, &vbo_triangle );
  glBindBuffer( GL_ARRAY_BUFFER, vbo_triangle );
  glBufferData( GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW );
	
	GLfloat tri_texcoords[] = {
		1.0, 1.0,	//Bot-Right
		0.0, 1.0,	//Bot-Left
		0.0, 0.0,	//Top-Left
		0.0, 0.0,	//Top-Left
		1.0, 0.0,	//Top-Right
		1.0, 1.0,	//Bot-Right
	};
	glGenBuffers( 1, &vbo_texcoords );
	glBindBuffer( GL_ARRAY_BUFFER, vbo_texcoords );
	glBufferData( GL_ARRAY_BUFFER, sizeof(tri_texcoords), tri_texcoords, GL_STATIC_DRAW );
	
  GLushort tri_elements[] = {
    0, 1, 2,
		3, 4, 5,
	};
	glGenBuffers( 1, &index_buffer );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, index_buffer );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(tri_elements), tri_elements, GL_STATIC_DRAW );
	
	SDL_Surface* res_texture = IMG_Load("filigree_W.png");
	if( res_texture == NULL ){
		cerr << "IMG_Load: " << SDL_GetError() << endl;
		return false;
	}
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D,	//target
								0,	//Level, 0 = no minimap
								GL_RGBA,	//Internal format
								res_texture->w,	//width
								res_texture->h,	//height
								0,	//Border, always 0 in OpenGL ES
								GL_RGBA,	//format
								GL_UNSIGNED_BYTE,	//type
								res_texture->pixels );
	SDL_FreeSurface( res_texture );
	
  //Vertex
  GLint link_ok = GL_FALSE;
	
  GLuint vs, fs;
  if( (vs = create_shader("tri.v.glsl", GL_VERTEX_SHADER)) == 0){
    return(false);
  }
  if( (fs = create_shader("tri.f.glsl", GL_FRAGMENT_SHADER)) == 0){
    return(false);
  }
  
  //Program and link check
  program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
  if( !link_ok ){
    cerr << "glLinkProgram:";
    print_log( program );
    return(false);
  }
  
  //
  const char* attribute_name = "v_position";
  attribute_coord2d = glGetAttribLocation( program, attribute_name );
  if( attribute_coord2d == -1 ){
    cerr << "Could not bind attribute " << attribute_name << endl;
    return(false);
  }
	attribute_name = "texcoord";
	attribute_texcoord = glGetAttribLocation( program, attribute_name );
	if( attribute_texcoord == -1 ){
		cerr << "Could not bind attribute " << attribute_name << endl;
		return false;
	}
	const char* uniform_name = "mytexture";
	uniform_mytexture = glGetUniformLocation( program, uniform_name );
	if( uniform_mytexture == -1 ){
		cerr << "Could not bind uniform " << uniform_name << endl;
		return false;
	}
  
  return true;
};

