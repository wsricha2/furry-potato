varying	vec2		f_UV_coord;
uniform	sampler2D	mytexture;

void main( void ){
	vec3 texel = texture2D( mytexture, f_UV_coord ).xyz;
	float x1 = texel[0];
	float x2 = texel[1];
	float x3 = texel[2];

	x1 = f_UV_coord.x * x1 - x2 + 0.5;
	x2 = x1 + f_UV_coord.y * x2 - x3;
	x3 = f_UV_coord.x * f_UV_coord.y * x3;

	gl_FragColor = vec4(x1, x2, x3, 1.0);
}
