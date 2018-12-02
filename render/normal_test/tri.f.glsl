varying	vec3			L;
varying	vec2			f_UV_coord;
uniform	sampler2D	mytexture;

void main( void ){
	vec3 N = normalize( texture2D( mytexture, f_UV_coord ).xyz * 2.0 - 1.0 );
	float Kd = max( dot( L, N ), 0.0 );
	gl_FragColor = vec4( Kd * vec3( 1.0, 0.0, 1.0), 1.0 );
}
