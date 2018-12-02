varying	float light_intensity;	

void main( void ){	
	gl_FragColor = vec4( light_intensity * vec3( 1.0, 0.0, 1.0 ), 1.0 );
}
