void render(SDL_Window* window){
  glClearColor( 0.5, 0.5, 0.5, 1.0 );
  glClear( GL_COLOR_BUFFER_BIT );
  
  glUseProgram( program );
  glEnableVertexAttribArray( attribute_coord2d );
  glBindBuffer( GL_ARRAY_BUFFER, vbo_triangle );
  glVertexAttribPointer( attribute_coord2d,
                         2,
                         GL_FLOAT,
                         GL_FALSE,
                         0,
                         0 );
  glDrawArrays( GL_TRIANGLES, 0, 3 );
  glDisableVertexAttribArray( attribute_coord2d );
  
  SDL_GL_SwapWindow( window );
};
