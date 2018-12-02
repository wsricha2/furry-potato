attribute	vec4	v_position;
attribute	vec2	UV_coords;

varying		vec2	f_UV_coord;

uniform		mat4	model_view;
uniform		mat4	projection;

void main(void){
	f_UV_coord = UV_coords;
	gl_Position = projection * model_view * v_position;
}
