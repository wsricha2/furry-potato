void render(SDL_Window* window){
  glClearColor( 0.5, 0.5, 0.5, 1.0 );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glActiveTexture( GL_TEXTURE0 );
	glUniform1i(uniform_mytexture, /*GL_TEXTURE*/0);
	glBindTexture( GL_TEXTURE_2D, texture_id );
  
  glUseProgram( program );
  glEnableVertexAttribArray( attribute_coord2d );
  glBindBuffer( GL_ARRAY_BUFFER, vbo_triangle );
  glVertexAttribPointer( attribute_coord2d,
                         3,
                         GL_FLOAT,
                         GL_FALSE,
                         0,
                         0 );
	glEnableVertexAttribArray( attribute_texcoord);
	glBindBuffer( GL_ARRAY_BUFFER, vbo_texcoords );
	glVertexAttribPointer( attribute_texcoord,
												 2,
												 GL_FLOAT,
												 GL_FALSE,
												 0,
												 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, index_buffer );
  glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );
  glDisableVertexAttribArray( attribute_coord2d );
  
  SDL_GL_SwapWindow( window );
};
