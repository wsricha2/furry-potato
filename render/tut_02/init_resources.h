GLuint program;
GLuint vbo_triangle;
GLint attribute_coord2d;

bool init_resources(void){
  GLfloat triangle_vertices[] = {
      0.0,  0.8,
     -0.8, -0.8,
      0.8, -0.8,
  };
  glGenBuffers(1, &vbo_triangle);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW );
  
  //Vertex
  GLint compile_ok = GL_FALSE, link_ok = GL_FALSE;
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
  const char* attribute_name = "coord2d";
  attribute_coord2d = glGetAttribLocation( program, attribute_name );
  if( attribute_coord2d == -1 ){
    cerr << "Could not bind attribute " << attribute_name << endl;
    return(false);
  }
  
  return true;
};

