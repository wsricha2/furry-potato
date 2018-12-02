#include <stdio.h>
#include <GL/glew.h>

int main(){
	FILE *file_handle = fopen("triangle.mesh", "wb");
	GLfloat triangle_vertices[] = {
		 0.8, -0.8,  0.0,
		-0.8, -0.8,  0.0,
		-0.8, -0.8,  0.0,
	};
	int triangle_count = 1;
	if( file_handle ){
		fwrite( &triangle_count, 1, sizeof( triangle_count ), file_handle );
		fwrite( &triangle_vertices, 1, sizeof( triangle_vertices ), file_handle );
		
		printf("success ~ %d", sizeof( triangle_vertices ) );
	}
	return( 0 );
}
