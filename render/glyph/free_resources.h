void free_resources(){
  glDeleteProgram( program );
  glDeleteBuffers( 1, &vbo_triangle );
  glDeleteBuffers( 1, &index_buffer );
};
