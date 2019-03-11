varying	vec2		f_UV_coord;
uniform	sampler2D	mytexture;

void main( void ){
	vec3 texel = texture2D( mytexture, f_UV_coord ).xyz;
	gl_FragColor = vec4( texel, 1.0 );
}
