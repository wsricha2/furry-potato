attribute	vec4	v_position;
attribute	vec2	UV_coords;

varying		vec2	f_UV_coord;

void main(void){
	f_UV_coord = UV_coords;
	gl_Position = v_position;
}
