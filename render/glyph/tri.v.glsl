attribute	vec3 v_position;
attribute	vec2 UV_coords;
varying		vec2 f_UV_coords;

void main(void){
	gl_Position = vec4(v_position, 1.0);
	f_UV_coords = UV_coords;
}
