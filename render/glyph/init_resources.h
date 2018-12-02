GLuint program;
GLuint vbo_triangle, index_buffer, UV_buffer;
GLint attribute_coord2d, attribute_UV_coords;
//Texture GLobals
GLuint texture_id, pogram_id;
GLint uniform_mytexture;

bool init_resources(void){
	FILE *file_handle = fopen("triangle.mesh", "rb");
	if(file_handle){
		
	}
	
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
	
	GLfloat UV_coords[] = {
//		0.0, 0.0,
//		0.5, 0.0,
//		1.0, 1.0,
		0.0, 0.0,
		0.5, 0.0,
		1.0, 1.0,
		0.0, 0.0,
		0.5, 0.0,
		1.0, 1.0,
	};
	glGenBuffers( 1, &UV_buffer );
	glBindBuffer( GL_ARRAY_BUFFER, UV_buffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(UV_coords), UV_coords, GL_STATIC_DRAW );

  GLushort tri_elements[] = {
    0, 1, 2,
		3, 4, 5,
	};
	glGenBuffers( 1, &index_buffer );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, index_buffer );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(tri_elements), tri_elements, GL_STATIC_DRAW );
	
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
  attribute_name = "UV_coords";
	attribute_UV_coords = glGetAttribLocation( program, attribute_name );
	if( attribute_UV_coords == -1 ){
		cerr << "Could not bind attribute " << attribute_name << endl;
		return false;
	}
	
  return true;
};

