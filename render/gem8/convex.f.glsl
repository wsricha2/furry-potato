varying vec2 UV_coords;

void main( void ){
	float U = UV_coords[ 0 ];
	float V = UV_coords[ 1 ];
	float implicit = U * U - V;
	float color;
	if( implicit < 0.0 ){
		color = 1.0;
	}
	else{
		color = 0.0;
	}
	
	gl_FragColor = vec4( 0.0, 0.0, 1.0, 1.0 );
}
