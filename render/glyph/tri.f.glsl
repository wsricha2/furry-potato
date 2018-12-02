varying vec2 f_UV_coords;

void main(void){
	float U = f_UV_coords[0];
	float V = f_UV_coords[1];
	float implicit = U * U - V;
	float color;
	if( implicit > 0){
		color = 1.0;
	}
	else{
		color = 0.0;
	}
	
	gl_FragColor = color * vec4( 1.0, 0.0, 1.0, 1.0 );
}
